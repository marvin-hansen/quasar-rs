// Compile with (MSVC): cl -I include qdb_api.lib ts-batch-insert.c
// Compile with (gcc/clang): gcc -I include -l libqdb_api.so ./ts-batch-insert.c
// Execute and later run query: select * from ts1, ts2

#include <qdb/query.h>
#include <qdb/ts.h>
#include <stdio.h>
#include <stdlib.h>

static qdb_error_t create_timeseries(qdb_handle_t h)
{
    qdb_ts_column_info_t columns[3];
    columns[0].name = "col1";
    columns[0].type = qdb_ts_column_double;
    columns[1].name = "col2";
    columns[1].type = qdb_ts_column_int64;
    columns[2].name = "col3";
    columns[2].type = qdb_ts_column_blob;

    qdb_error_t error = qdb_ts_create(h, "ts1", qdb_d_default_shard_size, columns, 2);
    if (error && (error != qdb_e_alias_already_exists)) return error;

    error = qdb_ts_create(h, "ts2", 4 * qdb_d_hour, columns, 3);
    if (error && (error != qdb_e_alias_already_exists)) return error;

    return qdb_e_ok;
}

static qdb_error_t batch_insert(qdb_handle_t h)
{
    qdb_batch_table_t table;
    qdb_ts_batch_column_info_t batch_columns[2];
    batch_columns[0].timeseries = "ts1";
    batch_columns[0].column     = "col2";
    batch_columns[1].timeseries = "ts2";
    batch_columns[1].column     = "col1";

    qdb_error_t error = qdb_ts_batch_table_init(h, batch_columns, 2, &table);
    if (error) return error;

    qdb_timespec_t timestamp;
    timestamp.tv_sec  = 1456234000;
    timestamp.tv_nsec = 0;

    for (int i = 0; i < 10; ++i)
    {
        error = qdb_ts_batch_start_row(table, &timestamp);
        if (error)
        {
            qdb_release(h, table);
            return error;
        }

        error = qdb_ts_batch_row_set_int64(table, /*index=*/0, (int)(42 * 3.14 * i)); // ts1, col2
        if (error)
        {
            qdb_release(h, table);
            return error;
        }
        error = qdb_ts_batch_row_set_double(table, /*index=*/1, 0.42001234 + 3.14 * i); // ts2, col1
        if (error)
        {
            qdb_release(h, table);
            return error;
        }

        ++timestamp.tv_sec;
        ++timestamp.tv_nsec;
        timestamp.tv_nsec %= 1000000000;
    }

    error = qdb_ts_batch_push(table);

    qdb_release(h, table);

    return error;
}

int main(int argc, char * argv[])
{
    qdb_handle_t handle;
    qdb_error_t error;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <uri>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char * url = argv[1];

    error = qdb_open(&handle, qdb_p_tcp);
    if (error)
    {
        fprintf(stderr, "qdb_open: %s (%#x)", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = qdb_connect(handle, url);
    if (error)
    {
        fprintf(stderr, "qdb_connect: %s (%#x)", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = create_timeseries(handle);
    if (error)
    {
        fprintf(stderr, "create timeseries: %s (%#x)", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = batch_insert(handle);
    if (error)
    {
        fprintf(stderr, "batch insert: %s (%#x)", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    qdb_close(handle);

    fprintf(stderr, "Success!");
    return EXIT_SUCCESS;
}
