/**
 * Copyright (c) 2009-2021, quasardb SAS. All rights reserved.
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

#ifndef __STDC_FORMAT_MACROS
#    define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <qdb/client.h>
#include <qdb/query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

qdb_error_t execute(const char * url, const char * query)
{
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (error) return error;

    error = qdb_connect(handle, url);
    if (error) return error;

    // doc-start-query
    qdb_query_result_t * result = NULL;

    error = qdb_query(handle, query, &result);
    if (error) return error;

    for (size_t ic = 0; ic < result->column_count; ++ic)
    {
        qdb_string_t * column = &result->column_names[ic];
        printf("%20s\t", column->data);
    }
    printf("\n");

    qdb_point_result_t ** rows = result->rows;
    for (size_t ir = 0; ir < result->row_count; ++ir)
    {
        printf("%5d:\t", (int)ir);

        qdb_point_result_t * row = rows[ir];
        for (size_t ic = 0; ic < result->column_count; ++ic)
        {
            qdb_point_result_t * cell = &row[ic];

            switch (cell->type)
            {
            case qdb_query_result_blob:
                printf("%15s", (const char *)cell->payload.blob.content);
                break;

            case qdb_query_result_count:
                printf("%15zu", cell->payload.count.value);
                break;

            case qdb_query_result_double:
                printf("%15f", cell->payload.double_.value);
                break;

            case qdb_query_result_int64:
                printf("%15" PRId64, cell->payload.int64_.value);
                break;

            case qdb_query_result_none:
                printf("(void)");
                break;

            case qdb_query_result_timestamp:
#if defined(macintosh) || defined(Macintosh) || defined(__APPLE__) || defined(__MACH__)
#    define PRItime "ld"
#else
#    define PRItime PRId64
#endif
                printf("%11" PRItime ".%09" PRItime, cell->payload.timestamp.value.tv_sec, cell->payload.timestamp.value.tv_nsec);
#undef PRItime
                break;

            case qdb_query_result_string:
                printf("%15s", cell->payload.string.content);
                break;
            }

            printf("\t");
        }

        printf("\n");
    }

    qdb_release(handle, result);
    // doc-end-query

    qdb_close(handle);

    return qdb_e_ok;
}

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <uri> <query>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char * url   = argv[1];
    const char * query = argv[2];

    qdb_error_t error = execute(url, query);
    if (error)
    {
        fprintf(stderr, "execute: %s (%#x)", qdb_error(error), error);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
