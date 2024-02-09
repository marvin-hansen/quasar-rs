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

#include <qdb/iterator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            // doc-start-iterator_begin
            qdb_const_iterator_t it;

            // Forward iteration.
            for (qdb_error_t error = qdb_iterator_begin(handle, &it); error == qdb_e_ok; error = qdb_iterator_next(&it))
            {
                // Work on the entry here:
                // * it.alias contains the entry alias (key)
                // * it.content and it.content_size is the entry content
            }

            qdb_iterator_close(&it);
            // doc-end-iterator_begin

            // doc-start-iterator_rbegin
            qdb_const_iterator_t rit;

            // Backward iteration.
            for (qdb_error_t error = qdb_iterator_rbegin(handle, &rit); error == qdb_e_ok; error = qdb_iterator_previous(&rit))
            {
                // Work on the entry here:
                // * rit.alias contains the entry alias (key)
                // * rit.content and rit.content_size is the entry content
            }

            qdb_iterator_close(&rit);
            // doc-end-iterator_rbegin
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
