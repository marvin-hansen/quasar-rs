/**
 * Copyright (c) 2009-2023, quasardb SAS. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of quasardb nor the names of its contributors may
 *      be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY QUASARDB AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <qdb/ts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    qdb_handle_t handle;
    qdb_error_t error;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <uri> <alias>\n", argv[0]);
        return EXIT_FAILURE;
    }

    error = qdb_open(&handle, qdb_p_tcp);
    if (error)
    {
        fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_open", qdb_error(error), error);
    }
    else
    {
        const char * url   = argv[1];
        const char * alias = argv[2];

        error = qdb_connect(handle, url);
        if (error)
        {
            fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_connect", qdb_error(error), error);
        }
        else
        {
            // doc-start-ts_create
            const qdb_ts_column_info_t my_columns[3u] = {
                {"my_double_column", qdb_ts_column_double},         //
                {"my_another_double_column", qdb_ts_column_double}, //
                {"my_blob_column", qdb_ts_column_blob}              //
            };
            error = qdb_ts_create(handle, alias, qdb_d_default_shard_size, my_columns, 3u);
            // doc-end-ts_create
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_create", qdb_error(error), error);
                return -1;
            }

            // doc-start-ts_insert_columns
            const qdb_ts_column_info_t my_additional_columns[2u] = {
                {"my_additional_double_column", qdb_ts_column_double}, //
                {"my_additional_blob_column", qdb_ts_column_blob}      //
            };
            error = qdb_ts_insert_columns(handle, alias, my_additional_columns, 2u);
            // doc-end-ts_insert_columns
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_create", qdb_error(error), error);
            }

            // doc-start-ts_column_helpers
            const qdb_ts_column_info_t double_column = my_columns[0];
            const qdb_ts_column_info_t blob_column   = my_columns[2];
            // doc-end-ts_column_helpers

            {
                // doc-start-ts_get_metadata
                qdb_ts_metadata_t * metadata;

                error = qdb_ts_get_metadata(handle, alias, &metadata);
                // doc-end-ts_get_metadata
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_get_metadata", qdb_error(error), error);
                }

                // doc-start-ts_metadata-release
                qdb_release(handle, metadata);
                // doc-end-ts_metadata-release
            }

            {
                // doc-start-ts_double_insert-single
                const qdb_ts_double_point my_double_point = {.timestamp = {.tv_sec = 0, .tv_nsec = 0}, .value = 1.234};

                error = qdb_ts_double_insert(handle, alias, double_column.name, &my_double_point, 1u);
                // doc-end-ts_double_insert-single
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_double_insert-simple", qdb_error(error), error);
                }
            }

            {
                // doc-start-ts_double_insert-multiple
                const qdb_ts_double_point my_double_points[5u] = {
                    {.timestamp = {.tv_sec = 563, .tv_nsec = 987654}, .value = 1.234}, //
                    {.timestamp = {.tv_sec = 564, .tv_nsec = 145654}, .value = -3e+3}, //
                    {.timestamp = {.tv_sec = 565, .tv_nsec = 715}, .value = 532e+3},   //
                    {.timestamp = {.tv_sec = 566, .tv_nsec = 6584}, .value = 4123e+4}, //
                    {.timestamp = {.tv_sec = 3563, .tv_nsec = 3845}, .value = 0.345}   //
                };
                error = qdb_ts_double_insert(handle, alias, double_column.name, my_double_points, 5u);
                // doc-end-ts_double_insert-multiple
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_double_insert", qdb_error(error), error);
                }
            }

            {
                // doc-start-ts_blob_insert
                const qdb_ts_blob_point my_blob_points[2u] = {
                    {.timestamp = {.tv_sec = 563, .tv_nsec = 987654}, .content = "hello", .content_length = 5},  //
                    {.timestamp = {.tv_sec = 3563, .tv_nsec = 3845}, .content = "quasardb", .content_length = 5} //
                };
                error = qdb_ts_blob_insert(handle, alias, blob_column.name, my_blob_points, 2u);
                // doc-end-ts_blob_insert
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_blob_insert", qdb_error(error), error);
                }
            }

            {
                // doc-start-ts_double_get_ranges
                const qdb_ts_range_t ranges[1u] = {{.begin = {.tv_sec = 0, .tv_nsec = 0}, .end = {.tv_sec = 0xFFFFFFFF, .tv_nsec = 0}}};
                qdb_ts_double_point * points    = NULL;
                qdb_size_t point_count          = 0;
                error = qdb_ts_double_get_ranges(handle, alias, double_column.name, ranges, 1u, &points, &point_count);
                // doc-end-ts_double_get_ranges
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_double_get_ranges", qdb_error(error), error);
                }

                // doc-start-ts_double_get_ranges-release
                qdb_release(handle, points);
                // doc-end-ts_double_get_ranges-release
            }

            /* Getting data */
            {
                // doc-start-bulk-get-ts_local_table_init
                qdb_local_table_t local_table;
                error = qdb_ts_local_table_init(handle, alias, my_columns, 3u, &local_table);
                // doc-end-bulk-get-ts_local_table_init
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_local_table_init", qdb_error(error), error);
                }

                // doc-start-bulk-get-ts_table_get_ranges
                qdb_ts_range_t range = {.begin = {.tv_sec = 0, .tv_nsec = 0}, .end = {.tv_sec = 1, .tv_nsec = 0}};
                error                = qdb_ts_table_get_ranges(local_table, &range, 1u);
                // doc-end-bulk-get-ts_table_get_ranges
                if (error || error == qdb_e_iterator_end)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_local_table_init", qdb_error(error), error);
                }
                else
                {
                    // doc-start-bulk-get-fetching
                    qdb_timespec_t timestamp;
                    while (!qdb_ts_table_next_row(local_table, &timestamp))
                    {
                        printf("timestamp: %lu s - %lu ns\n", (unsigned long)timestamp.tv_sec, (unsigned long)timestamp.tv_nsec);
                        // respect the order of the columns
                        double value = 0;
                        if (!qdb_ts_row_get_double(local_table, 0, &value))
                        {
                            printf("column[0]: %f\n", value);
                        }
                        else
                        {
                            // handle error
                        }

                        if (!qdb_ts_row_get_double(local_table, 1, &value))
                        {
                            printf("column[1]: %f\n", value);
                        }
                        else
                        {
                            // handle error
                        }

                        const void * content = NULL;
                        qdb_size_t content_length;
                        if (!qdb_ts_row_get_blob(local_table, 2, &content, &content_length))
                        {
                            printf("column[2]: %s\n", (const char *)content);
                        }
                        else
                        {
                            // handle error
                        }

                        // release memory
                        qdb_release(handle, content);
                    }
                    // doc-end-bulk-get-fetching
                }

                // doc-start-bulk-get-release
                // don't forget to release the table once finished
                qdb_release(handle, local_table);
                // doc-start-bulk-get-release
            }

            {
                // doc-start-ts_double_aggregate
                qdb_ts_double_aggregation_t double_aggregations[1u] = {{//
                    .type  = qdb_agg_arithmetic_mean,                   //
                    .range = {.begin = {.tv_sec = 0, .tv_nsec = 0}, .end = {.tv_sec = 0xFFFFFFFF, .tv_nsec = 0}}}};
                error = qdb_ts_double_aggregate(handle, alias, double_column.name, double_aggregations, 1u);
                // doc-end-ts_double_aggregate
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_double_aggregate", qdb_error(error), error);
                }
                else
                {
                    // doc-start-ts_double_aggregate-printf
                    printf("Double average in column %s: %d\n", double_column.name, (int)double_aggregations[0].result.value);
                    // doc-end-ts_double_aggregate-printf
                }
            }

            {
                // doc-start-ts_blob_aggregate
                qdb_ts_blob_aggregation_t blob_aggregations[1u] = {{
                    //
                    .type  = qdb_agg_count, //
                    .range = {.begin = {.tv_sec = 0, .tv_nsec = 0}, .end = {.tv_sec = 0xFFFFFFFF, .tv_nsec = 0}},
                }};
                error = qdb_ts_blob_aggregate(handle, alias, blob_column.name, blob_aggregations, 1u);
                // doc-end-ts_blob_aggregate
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_blob_aggregate", qdb_error(error), error);
                }
                else
                {
                    // doc-start-ts_blob_aggregate-printf
                    printf("Blob count: %d\n", (int)blob_aggregations[0].count);
                    // doc-end-ts_blob_aggregate-printf
                }
            }
            {
                // doc-start-ts_erase_ranges
                const qdb_ts_range_t ranges[1u] = {{.begin = {.tv_sec = 0, .tv_nsec = 0}, .end = {.tv_sec = 0xFFFFFFFF, .tv_nsec = 0}}};

                qdb_uint_t erased_count = 0;
                error                   = qdb_ts_erase_ranges(handle, alias, double_column.name, ranges, 1u, &erased_count);
                // doc-end-ts_erase_ranges
                if (error)
                {
                    fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_ts_erase_ranges", qdb_error(error), error);
                }
            }

            // doc-start-ts_remove
            error = qdb_remove(handle, alias);
            // doc-end-ts_remove
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_remove", qdb_error(error), error);
            }
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
