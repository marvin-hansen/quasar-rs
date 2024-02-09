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

#ifndef __STDC_FORMAT_MACROS
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#    define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <qdb/client.h>
#include <qdb/query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// callback-start
static int query_callback(void * p, qdb_error_t err, const qdb_query_result_t * result)
{
    (void)p; // unused in this example

    if (QDB_FAILURE(err))
    {
        printf("Error received: %s\n", qdb_error(err));
        return 0;
    }

    // access the results only if there is no error
    printf("%d column(s) and %d row(s) received!\n", (int)result->column_count, (int)result->row_count);

    // return 0, unless you want the continuous queries to end
    return 0;
}
// callback-end

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <uri> <query>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char * url   = argv[1];
    const char * query = argv[2];

    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (QDB_SUCCESS(error))
    {
        error = qdb_connect(handle, url);
    }

    if (QDB_FAILURE(error))
    {
        fprintf(stderr, "Cannot connect to %s: %s (%#x)\n", query, qdb_error(error), error);
        return EXIT_FAILURE;
    }

    printf("Connected to %s\n", url);

    // continuous-start
    qdb_query_cont_handle_t cont_query = NULL;
    const int refresh_rate_in_ms       = 10000;
    error = qdb_query_continuous(handle, query, qdb_query_continuous_full, refresh_rate_in_ms, &query_callback, NULL, &cont_query);
    if (QDB_FAILURE(error))
    {
        fprintf(stderr, "Cannot run continuous query %s: %s (%#x)\n", query, qdb_error(error), error);
        return EXIT_FAILURE;
    }
    // continuous-end

    printf("Executing \"%s\"\n", query);
    printf("Press ENTER to exit...\n");

    getchar();

    // cleanup-start
    qdb_release(handle, cont_query);
    qdb_close(handle);
    // cleanup-end

    return EXIT_SUCCESS;
}
