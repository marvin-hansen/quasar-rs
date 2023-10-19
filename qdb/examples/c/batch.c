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

#include <qdb/batch.h>
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
            // doc-start-init_operations
            const size_t operation_count = 4;
            qdb_operation_t ops[4];
            error = qdb_init_operations(ops, operation_count);
            if (error)
            {
                // error management
            }
            // doc-end-init_operations
            else
            {
                // doc-start-batch-create
                // the first operation will be a get for "entry1"
                ops[0].type  = qdb_op_blob_get;
                ops[0].alias = "entry1";

                // the second operation will be a get for "entry2"
                ops[1].type  = qdb_op_blob_get;
                ops[1].alias = "entry2";

                // the third operation will be an update for "entry3"
                const size_t content_size = 100;

                char content[100];
                ops[2].type                     = qdb_op_blob_update;
                ops[2].alias                    = "entry3";
                ops[2].blob_update.content      = content;
                ops[2].blob_update.content_size = content_size;

                // the fourth operation will be increasing an integer "int_value" by 42
                ops[3].type           = qdb_op_int_add;
                ops[3].alias          = "int_value";
                ops[3].int_add.addend = 42;
                // doc-end-batch-create

                // doc-start-run_batch
                // Runs the batch on the cluster and returns the count of successfully executed operations.
                size_t success_count = qdb_run_batch(handle, ops, operation_count);
                // doc-end-run_batch

                // doc-start-error
                if (success_count != operation_count)
                {
                    for (qdb_size_t i = 0; i < operation_count; ++i)
                    {
                        if ((ops[i].error != qdb_e_ok) && (ops[i].error != qdb_e_ok_created))
                        {
                            // we have an error in this operation
                        }
                    }
                }
                // doc-end-error

                // doc-start-results-blob
                const char * entry1_content = ops[0].blob_get.content;
                qdb_size_t entry1_size      = ops[0].blob_get.content_size;

                const char * entry2_content = ops[1].blob_get.content;
                qdb_size_t entry2_size      = ops[1].blob_get.content_size;
                // doc-end-results-blob
                (void)entry1_content; // unused
                (void)entry1_size;    // unused
                (void)entry2_content; // unused
                (void)entry2_size;    // unused

                // doc-start-results-int
                qdb_int_t result = ops[3].int_add.result;
                // doc-end-results-int
                (void)result; // unused

                // doc-start-free_operations
                qdb_release(handle, ops);
                // doc-end-free_operations
            }
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
