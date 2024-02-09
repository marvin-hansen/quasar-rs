// import-start
#include <qdb/client.h>
#include <qdb/tag.h>
#include <qdb/ts.h>
// import-end

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// NOTE: Name connect may clash with system-header ::connect.
int db_connect(qdb_handle_t * h)
{
    assert(h);

    // connect-start
    // We first need to open a handle, which is is the memory structure that
    // QuasarDB uses to maintain connection state.
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // Now that we have opened the handle, we can tell it to establish a connection
    // with the cluster.
    error = qdb_connect(handle, "qdb://localhost:2836");
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // connect-end

    *h = handle;

    return EXIT_SUCCESS;
}

int secure_db_connect(qdb_handle_t * h)
{
    assert(h);

    // secure-connect-start
    // We first need to open a handle, which is is the memory structure that
    // QuasarDB uses to maintain connection state.
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // Load the encoded key
    error = qdb_option_set_cluster_public_key(handle, "cluster_public_key");
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // Then the username and its associated encoded key
    error = qdb_option_set_user_credentials(handle, "user", "user_private_key");
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // another option is to load directly from the security files
    error = qdb_option_load_security_files(handle, "cluster_public_key.txt", "user_credentials.txt");

    // Now that we have opened the handle, we can tell it to establish a connection
    // with the cluster.
    error = qdb_connect(handle, "qdb://localhost:2836");
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // secure-connect-end

    *h = handle;

    return EXIT_SUCCESS;
}

int main(void)
{
    qdb_handle_t handle = NULL;

    if (db_connect(&handle)) return EXIT_FAILURE;

    // create-table-start
    // Initialize our columns definitions
    const qdb_ts_column_info_t columns[3] = {
        {.name = "open", .type = qdb_ts_column_double},  //
        {.name = "close", .type = qdb_ts_column_double}, //
        {.name = "volume", .type = qdb_ts_column_int64}  //
    };
    const int columns_count = sizeof(columns) / sizeof(qdb_ts_column_info_t);

    // Now create the table with the default shard size
    qdb_error_t error = qdb_ts_create(handle, "stocks", qdb_d_default_shard_size, columns, columns_count);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // create-table-end

    // tags-start
    error = qdb_attach_tag(handle, "stocks", "nasdaq");
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // tags-end

    // batch-insert-start
    // Initialize our batch columns definitions
    const qdb_ts_batch_column_info_t batch_columns[3] = {
        {.timeseries = "stocks", .column = "open", .elements_count_hint = 2},  //
        {.timeseries = "stocks", .column = "close", .elements_count_hint = 2}, //
        {.timeseries = "stocks", .column = "volume", .elements_count_hint = 2} //
    };
    const int batch_columns_count = sizeof(batch_columns) / sizeof(qdb_ts_batch_column_info_t);

    // create our batch handle
    qdb_batch_table_t table;
    error = qdb_ts_batch_table_init(handle, batch_columns, batch_columns_count, &table);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // The batch API is row oriented, we first setup the start timestamp of the row
    // Set timestamp to 2019-02-01
    qdb_timespec_t timestamp = {.tv_sec = 1548979200, .tv_nsec = 0};
    qdb_ts_batch_start_row(table, &timestamp);

    // Then set the values for each column
    qdb_ts_batch_row_set_double(table, 0, 3.40);
    qdb_ts_batch_row_set_double(table, 1, 3.50);
    qdb_ts_batch_row_set_int64(table, 2, 10000);

    // Add another row
    // Set timestamp to 2019-02-02
    timestamp.tv_sec = 1549065600;
    qdb_ts_batch_start_row(table, &timestamp);
    qdb_ts_batch_row_set_double(table, 0, 3.50);
    qdb_ts_batch_row_set_double(table, 1, 3.55);
    qdb_ts_batch_row_set_int64(table, 2, 7500);

    // Push into the database as a single operation
    error = qdb_ts_batch_push(table);

    // Don't forget to release the table
    qdb_release(handle, table);
    // batch-insert-end

    // bulk-read-start
    // We can initialize our bulk reader directly from the columns we defined earlier
    qdb_local_table_t local_table;
    error = qdb_ts_local_table_init(handle, "stocks", columns, columns_count, &local_table);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;

    // Setup a range going from 2019-02-01 to 2019-02-02
    qdb_ts_range_t range = {.begin = {.tv_sec = 1548979200, .tv_nsec = 0}, .end = {.tv_sec = 1549065600, .tv_nsec = 0}};
    error                = qdb_ts_table_get_ranges(local_table, &range, 1u);

    while (!qdb_ts_table_next_row(local_table, &timestamp))
    {
        double value_index_zero   = 0;
        double value_index_one    = 0;
        qdb_int_t value_index_two = 0;

        error = qdb_ts_row_get_double(local_table, 0, &value_index_zero);
        // put cleanup logic here in case of error
        error = qdb_ts_row_get_double(local_table, 1, &value_index_one);
        // put cleanup logic here in case of error
        error = qdb_ts_row_get_int64(local_table, 2, &value_index_two);
        // put cleanup logic here in case of error
    }

    // don't forget to release the table once finished
    qdb_release(handle, local_table);
    // bulk-read-end

    // column-insert-start
    // Prepare the points for each column
    const qdb_ts_double_point opens[2] = {
        {.timestamp = {.tv_sec = 1548979200, .tv_nsec = 0}, .value = 3.4}, //
        {.timestamp = {.tv_sec = 1549065600, .tv_nsec = 0}, .value = 3.5}  //
    };
    const qdb_ts_double_point closes[2] = {
        {.timestamp = {.tv_sec = 1548979200, .tv_nsec = 0}, .value = 3.50}, //
        {.timestamp = {.tv_sec = 1549065600, .tv_nsec = 0}, .value = 3.55}  //
    };
    const qdb_ts_int64_point volumes[2] = {
        {.timestamp = {.tv_sec = 1548979200, .tv_nsec = 0}, .value = 7500}, //
        {.timestamp = {.tv_sec = 1549065600, .tv_nsec = 0}, .value = 10000} //
    };

    // Insert each column independently
    error = qdb_ts_double_insert(handle, "stocks", "open", opens, 2u);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    error = qdb_ts_double_insert(handle, "stocks", "close", closes, 2u);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    error = qdb_ts_int64_insert(handle, "stocks", "volume", volumes, 2u);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // column-insert-end

    // column-get-start
    // Setup the range(s) we want to get
    const qdb_ts_range_t ranges[1] = {{.begin = {.tv_sec = 1548979200, .tv_nsec = 0}, .end = {.tv_sec = 1549065600, .tv_nsec = 0}}};

    // We write the data into empty structure you pass as in-out parameters
    qdb_ts_double_point * points = NULL;
    qdb_size_t point_count       = 0;

    // Get the provided ranges
    error = qdb_ts_double_get_ranges(handle, "stocks", "open", ranges, 1u, &points, &point_count);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // column-get-end

    // drop-table-start
    // A timeseries is considered a normal entry for this operation
    // You can safely remove it
    qdb_remove(handle, "stocks");
    // drop-table-end

    // close-start
    error = qdb_close(handle);
    if (QDB_FAILURE(error)) return EXIT_FAILURE;
    // close-end

    return EXIT_SUCCESS;
}
