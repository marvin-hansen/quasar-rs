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

#include <qdb/blob.h>
#include <qdb/option.h>
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
    if (!error)
    {
        error = qdb_connect(handle, argv[1]);
        if (!error)
        {
            // doc-start-option_set_timeout
            // Set the timeout to 5 seconds (5000 ms).
            qdb_option_set_timeout(handle, 5000);
            // doc-end-option_set_timeout

            // doc-start-blob_get
            const char * alias = argv[2];
            const void * allocated_content;
            qdb_size_t allocated_content_length = 0u;
            error                               = qdb_blob_get(handle, alias, &allocated_content, &allocated_content_length);
            if (error)
            {
                // error management
            }
            // doc-end-blob_get
            else
            {
                fwrite(allocated_content, allocated_content_length, 1, stdout);
                // doc-start-free_buffer
                qdb_release(handle, allocated_content);
                // doc-end-free_buffer
            }
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
