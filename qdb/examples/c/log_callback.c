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

#include <qdb/client.h>
#include <qdb/log.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char * get_log_level_as_string(qdb_log_level_t log_level)
{
    // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
    switch (log_level)
    {
    case qdb_log_detailed:
        return "detailed";
    case qdb_log_debug:
        return "debug";
    case qdb_log_info:
        return "info";
    case qdb_log_warning:
        return "warning";
    case qdb_log_error:
        return "error";
    case qdb_log_panic:
        return "panic";
    default:
        assert(!"unknown enum value");
        return "unknown";
    }
}

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201112L)) || (defined(__cplusplus) && (__cplusplus > 201103L))
// doc-start-log_callback
typedef void (*qdb_log_callback)( //
    qdb_log_level_t log_level,    // qdb log level
    const unsigned long * date,   // [years, months, day, hours, minute, seconds] (valid only in the context of the callback)
    unsigned long pid,            // process id
    unsigned long tid,            // thread id
    const char * message_buffer,  // message buffer (valid only in the context of the callback)
    size_t message_size);         // message buffer size
// doc-end-log_callback
#endif

// doc-start-my_log_callback
void my_log_callback(qdb_log_level_t log_level,
    const unsigned long * date,
    unsigned long pid,
    unsigned long tid,
    const char * message_buffer,
    size_t message_size)
{
    // will print to the console the log message, e.g.
    // 12/31/2013-23:12:01    debug (   12: 1234): here is the message
    // note that you don't have to use all provided information, only use what you need!
    printf("%02lu/%02lu/%04lu-%02lu:%02lu:%02lu %8s (%5lu:%5lu): %.*s\n", date[1], date[2], date[0], date[3], date[4], date[5],
        get_log_level_as_string(log_level), pid, tid, (int)message_size, message_buffer);
}
// doc-end-my_log_callback

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
    if (!error)
    {
        error = qdb_connect(handle, url);
        if (!error)
        {
            // doc-start-log_add_callback
            qdb_log_callback_id cid = 0;
            error                   = qdb_log_add_callback(my_log_callback, &cid);
            // doc-end-log_add_callback
            if (error)
            {
                // error management
            }
            else
            {
                // doc-start-log_remove_callback
                error = qdb_log_remove_callback(cid);
                // doc-end-log_remove_callback
                if (error)
                {
                    // error management
                }
            }

            // doc-start-log_add_callback-no-cid
            // Giving no callback id prevents you from removing the callback in the future if the need be.
            error = qdb_log_add_callback(my_log_callback, NULL);
            // doc-end-log_add_callback-no-cid
            if (error)
            {
                // error management
            }
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
