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
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    qdb_handle_t handle;
    qdb_error_t error;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <uri> <alias>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char * url   = argv[1];
    const char * alias = argv[2];

    error = qdb_open(&handle, qdb_p_tcp);
    if (!error)
    {
        error = qdb_connect(handle, url);
        if (!error)
        {
            // doc-start-remove
            error = qdb_remove(handle, alias);
            if (error)
            {
                // error management
            }
            // doc-end-remove
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
