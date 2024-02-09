// Compile with (MSVC): cl -I include qdb_api.lib ts-batch-insert.c
// Compile with (gcc/clang): gcc -I include -l libqdb_api.so ./ts-batch-insert.c
// Execute and later run query: select * from ts1, ts2

#include <qdb/client.h>
#include <qdb/ts.h>
#include <stdio.h>
#include <stdlib.h>

static qdb_error_t create_timeseries(qdb_handle_t h)
{
    qdb_ts_column_info_t columns[2];
    columns[0].name = "col1";
    columns[0].type = qdb_ts_column_int64;
    columns[1].name = "col2";
    columns[1].type = qdb_ts_column_double;

    qdb_error_t error = qdb_ts_create(h, "ts1", qdb_d_default_shard_size, columns, 2);
    if (error && (error != qdb_e_alias_already_exists)) return error;

    return qdb_e_ok;
}

static qdb_error_t exp_batch_insert(qdb_handle_t h)
{
    const qdb_timespec_t timestamps[2] = {{.tv_sec = 1548979200, .tv_nsec = 0}, {.tv_sec = 1548979201, .tv_nsec = 0}};
    const qdb_int_t data_int64[2]      = {1, 2};
    const double data_double[2]        = {1.1, 2.2};

    qdb_exp_batch_push_column_t columns[2u] = {
        {.name = (qdb_string_t){"col1", 4}, .data_type = qdb_ts_column_int64},
        {.name = (qdb_string_t){"col2", 4}, .data_type = qdb_ts_column_double},
    };

    qdb_exp_batch_push_table_t table = {
        .name    = (qdb_string_t){"ts1", 3},
        .options = qdb_exp_batch_option_standard,
        .data =
            {
                .timestamps   = timestamps,
                .columns      = columns,
                .column_count = 2u,
            },
    };

    columns[0].data.ints    = data_int64;
    columns[1].data.doubles = data_double;
    table.data.row_count    = 2u;

    qdb_error_t error = qdb_exp_batch_push(h, qdb_exp_batch_push_transactional, &table, NULL, 1u);
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
        fprintf(stderr, "qdb_open: %s (%#x)\n", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = qdb_connect(handle, url);
    if (error)
    {
        fprintf(stderr, "qdb_connect: %s (%#x)\n", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = create_timeseries(handle);
    if (error)
    {
        fprintf(stderr, "create timeseries: %s (%#x)\n", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    error = exp_batch_insert(handle);
    if (error)
    {
        fprintf(stderr, "exp_batch insert: %s (%#x)\n", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    qdb_close(handle);

    fprintf(stderr, "Success!\n");
    return EXIT_SUCCESS;
}
