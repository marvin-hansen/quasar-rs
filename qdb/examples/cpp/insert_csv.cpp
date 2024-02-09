#include <qdb/client.hpp>
#include <qdb/ts.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/predef/os/windows.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <variant>

static constexpr qdb_size_t expected_col_count = 5;
static const std::array<qdb_ts_column_info_t, expected_col_count> expected_cols{qdb_ts_column_info_t{"open", qdb_ts_column_double},
    qdb_ts_column_info_t{"high", qdb_ts_column_double}, qdb_ts_column_info_t{"low", qdb_ts_column_double},
    qdb_ts_column_info_t{"close", qdb_ts_column_double}, qdb_ts_column_info_t{"volume", qdb_ts_column_int64}};

static void open_time_series(qdb::handle & h, const char * const time_series)
{
    qdb_ts_metadata_t * metadata = nullptr;

    qdb_error_t err = qdb_ts_get_metadata(h, time_series, &metadata);
    if (err == qdb_e_alias_not_found)
    {
        // create the open/high/low/close/volume time series
        err = qdb_ts_create(h, time_series, qdb_uint_t{qdb_d_day} * 30, expected_cols.data(), expected_cols.size());
        if (QDB_FAILURE(err)) throw err;

        return;
    }

    if (QDB_FAILURE(err)) throw err;
    qdb_size_t existing_cols_count = metadata->column_count;

    qdb_release(h, metadata);

    if (existing_cols_count != expected_col_count) throw std::runtime_error("Unexpected columns count");
}

static constexpr int char_to_int(char c) noexcept
{
    return c - 0x30;
}

static int parse_four_digits(const std::string & str, size_t index) noexcept
{
    return char_to_int(str[index]) * 1000 + char_to_int(str[index + 1]) * 100 + char_to_int(str[index + 2]) * 10
           + char_to_int(str[index + 3]);
}

static int parse_two_digits(const std::string & str, size_t index) noexcept
{
    return char_to_int(str[index]) * 10 + char_to_int(str[index + 1]);
}

std::tuple<qdb_timespec_t, int> parse_timestamp(const std::string & date, const std::string & time)
{
    // expects mm/dd/yyyy for date
    if (date.size() != 10) throw std::runtime_error("invalid date token size");
    if (time.size() != 4) throw std::runtime_error("invalid time token size");

    std::tm time_struct;
    memset(&time_struct, 0, sizeof(time_struct));

    time_struct.tm_mon = parse_two_digits(date, 0);
    if ((time_struct.tm_mon < 1) || (time_struct.tm_mon > 12)) throw std::runtime_error("could not parse month");

    time_struct.tm_mday = parse_two_digits(date, 3);
    if ((time_struct.tm_mday < 1) || (time_struct.tm_mday > 31)) throw std::runtime_error("could not parse day");

    time_struct.tm_year = parse_four_digits(date, 6);
    if ((time_struct.tm_year < 1900) || (time_struct.tm_year >= 10'000)) throw std::runtime_error("could not parse year");

    // expects hhmm for time
    time_struct.tm_hour = parse_two_digits(time, 0);
    if (time_struct.tm_hour > 23) throw std::runtime_error("could not parse hour");

    time_struct.tm_min = parse_two_digits(time, 2);
    if (time_struct.tm_min > 59) throw std::runtime_error("could not parse minutes");

    time_struct.tm_year -= 1900; // years after 1900
    --time_struct.tm_mon;        // [0-11] month

    qdb_timespec_t result;

#if BOOST_OS_WINDOWS
    // mktime works on localtime, not gmtime, we need to set the zone manually
    result.tv_sec = static_cast<qdb_time_t>(std::mktime(&time_struct) - _timezone);
#else
    // timegm is a non-standard FreeBSD function that converts a time struct without assuming it's Zulu time
    result.tv_sec = static_cast<qdb_time_t>(::timegm(&time_struct));
#endif

    result.tv_nsec = 0;

    return std::make_tuple(result, time_struct.tm_mday);
}

using column_type = std::variant<std::monostate, std::vector<qdb_ts_double_point>, std::vector<qdb_ts_int64_point>>;
using columns     = std::vector<column_type>;

struct column_inserter
{
    column_inserter(qdb_handle_t h, const char * ts_name, const char * col_name, size_t rows_count)
        : _handle{h}
        , _ts_name{ts_name}
        , _col_name{col_name}
        , _rows_count{rows_count}
    {}

    qdb_error_t operator()(const std::vector<qdb_ts_double_point> & pt) const noexcept
    {
        return qdb_ts_double_insert(_handle, _ts_name, _col_name, pt.data(), _rows_count);
    }

    qdb_error_t operator()(const std::vector<qdb_ts_int64_point> & pt) const noexcept
    {
        return qdb_ts_int64_insert(_handle, _ts_name, _col_name, pt.data(), _rows_count);
    }

    qdb_error_t operator()(std::monostate /*unused*/) const noexcept
    {
        return qdb_e_internal_local;
    }

private:
    qdb_handle_t _handle;
    const char * _ts_name;
    const char * _col_name;
    size_t _rows_count;
};

#ifdef QDB_INSERT_MULTITHREADED
static void insert_columns_multithreaded(qdb::handle & h, const columns & cols, qdb_size_t rows_count, const char * time_series)
{
    const size_t cols_count = cols.size();

    std::vector<std::thread> tasks;

    tasks.reserve(cols_count);

    // we should be using a cache aligned allocator
    std::vector<qdb_error_t> errors(cols_count, qdb_e_uninitialized);

    size_t i = 0;
    for (const auto & col : cols)
    {
        tasks.emplace_back([i, &h, time_series, &col, &errors, rows_count] {
            assert(i < errors.size());
            errors[i] = std::visit(column_inserter{h, time_series, expected_cols[i].name, rows_count}, col);
        });

        ++i;
    }

    for (auto & t : tasks)
    {
        t.join();
    }

    tasks.clear();

    auto it = std::find_if(errors.cbegin(), errors.cend(), [](qdb_error_t err) {
        return QDB_FAILURE(err);
    });
    if (it != errors.cend())
    {
        throw *it;
    }
}
#else
static void insert_columns_monothreaded(qdb::handle & h, const columns & cols, qdb_size_t rows_count, const char * time_series)
{
    size_t i = 0;

    for (const auto & col : cols)
    {
        qdb_error_t err = std::visit(column_inserter{h, time_series, expected_cols[i++].name, rows_count}, col);
        if (QDB_FAILURE(err)) throw err;
    }
}
#endif

static void insert_columns(qdb::handle & h, const columns & cols, qdb_size_t rows_count, const char * time_series)
{
#ifdef QDB_INSERT_MULTITHREADED
    insert_columns_multithreaded(h, cols, rows_count, time_series);
#else
    insert_columns_monothreaded(h, cols, rows_count, time_series);
#endif
}

template <size_t Count>
static columns make_columns(const std::array<qdb_ts_column_info_t, Count> & cols, size_t rows_count)
{
    columns result;

    for (const qdb_ts_column_info_t & col : cols)
    {
        switch (col.type)
        {
        case qdb_ts_column_double:
            result.emplace_back(std::vector<qdb_ts_double_point>(rows_count));
            break;

        case qdb_ts_column_int64:
            result.emplace_back(std::vector<qdb_ts_int64_point>(rows_count));
            break;

        default:
            throw qdb_e_not_implemented;
        }
    }

    return result;
}

struct column_loader
{
    column_loader(qdb_size_t row_index, qdb_timespec_t ts, const std::string & v)
        : _row_index{row_index}
        , _timestamp{ts}
        , _value{v}
    {}

    void operator()(std::vector<qdb_ts_int64_point> & v) const
    {
        v[_row_index].timestamp = _timestamp;
        v[_row_index].value     = static_cast<qdb_int_t>(std::stoll(_value));
    }

    void operator()(std::vector<qdb_ts_double_point> & v) const
    {
        v[_row_index].timestamp = _timestamp;
        v[_row_index].value     = std::stod(_value);
    }

    void operator()(std::monostate /*unused*/) const
    {
        throw qdb_e_internal_local;
    }

private:
    qdb_size_t _row_index;
    qdb_timespec_t _timestamp;
    const std::string & _value;
};

static void load_csv(qdb::handle & h, std::ifstream & csv_file, const char * time_series)
{
    static constexpr int days_per_batch  = 180;
    static constexpr int minutes_per_day = 24 * 60;

    std::string line;

    std::vector<std::string> tokens;

    // skip the first line, the header line
    if (auto && is = std::getline(csv_file, line); !is)
    {
        std::cerr << "error while skipping header!" << std::endl;
        return;
    }

    const auto start = std::chrono::high_resolution_clock::now();

    int previous_day = -1;
    int days_count   = 0;

    columns cols_to_insert = make_columns(expected_cols, days_per_batch * minutes_per_day);

    qdb_size_t total_rows      = 0;
    qdb_size_t this_batch_rows = 0;

    while (std::getline(csv_file, line))
    {
        boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(","));

        if (tokens.size() != (expected_col_count + 2)) throw std::runtime_error("invalid line length");

        auto [row_ts, this_day] = parse_timestamp(tokens[0], tokens[1]);

        if (this_day != previous_day)
        {
            ++days_count;
        }

        previous_day = this_day;

        if (days_count >= days_per_batch)
        {
            std::cout << "pushing " << this_batch_rows << " rows - now at " << total_rows << " rows..." << std::endl;

            insert_columns(h, cols_to_insert, this_batch_rows, time_series);

            days_count      = 0;
            this_batch_rows = 0;
        }

        for (qdb_size_t i = 0; i < expected_col_count; ++i)
        {
            std::visit(column_loader{this_batch_rows, row_ts, tokens[i + 2]}, cols_to_insert[i]);
        }

        ++this_batch_rows;
        ++total_rows;
    }

    const auto stop = std::chrono::high_resolution_clock::now();

    if (this_batch_rows > 0)
    {
        std::cout << "pushing " << this_batch_rows << " rows - now at " << total_rows << " rows..." << std::endl;
        insert_columns(h, cols_to_insert, this_batch_rows, time_series);
    }

    const auto duration          = stop - start;
    using fp_seconds             = std::chrono::duration<double>;
    const fp_seconds fp_duration = duration;

    const auto rows_per_sec   = static_cast<qdb_size_t>(total_rows / fp_duration.count());
    const auto points_per_sec = static_cast<qdb_size_t>(total_rows * expected_col_count / fp_duration.count()); // NOLINT

    std::cout << "Loaded and uploaded:\n";
    std::cout << " - " << total_rows << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms\n";
    std::cout << " - " << rows_per_sec << " rows per second\n";
    std::cout << " - " << points_per_sec << " points per second\n";
}

int main(int argc, char ** argv)
{
    try
    {
        static constexpr int expected_args = 4;

        if (argc < expected_args)
        {
            std::cerr << "usage: " << argv[0] << " qdb_url csv_file time_series" << std::endl;
            return EXIT_FAILURE;
        }

        qdb::handle h;

        std::cout << "connecting to " << argv[1] << "..." << std::endl;

        qdb_error_t err = h.connect(argv[1]);
        if (QDB_FAILURE(err))
        {
            throw err;
        }

        std::cout << "opening " << argv[2] << "..." << std::endl;
        std::ifstream csv_file{argv[2]};

        if (csv_file.fail()) throw std::runtime_error("Cannot open file");

        std::cout << "accessing time series " << argv[3] << "..." << std::endl;
        open_time_series(h, argv[3]);

        std::cout << "uploading data..." << std::endl;
        load_csv(h, csv_file, argv[3]);

        return EXIT_SUCCESS;
    }
    catch (qdb_error_t err)
    {
        std::cerr << "quasardb error: " << qdb_error(err) << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception & e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
