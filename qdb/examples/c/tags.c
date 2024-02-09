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
#include <qdb/tag.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    qdb_handle_t handle;
    qdb_error_t error;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <uri> <alias> <content>\n", argv[0]);
        return EXIT_FAILURE;
    }

    error = qdb_open(&handle, qdb_p_tcp);
    if (error)
    {
        fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_open", qdb_error(error), error);
    }
    else
    {
        const char * url     = argv[1];
        const char * alias   = argv[2];
        const char * content = argv[3];

        const char * tag = "my_tag";

        error = qdb_connect(handle, url);
        if (error)
        {
            fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_connect", qdb_error(error), error);
        }
        else
        {
            error = qdb_blob_put(handle, alias, (const void *)content, strlen(content), qdb_never_expires);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_blob_put", qdb_error(error), error);
            }

            // doc-start-tag_attach
            error = qdb_attach_tag(handle, alias, tag);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_attach_tag", qdb_error(error), error);
            }

            const char * tags_to_attach[2] = {"my_tag1", "my_tag2"};

            error = qdb_attach_tags(handle, alias, tags_to_attach, 2);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_attach_tags", qdb_error(error), error);
            }
            // doc-end-tag_attach

            // doc-start-tag_detach
            error = qdb_detach_tag(handle, alias, tag);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_detach_tag", qdb_error(error), error);
            }

            const char * tags_to_detach[2] = {"my_tag1", "my_tag2"};

            error = qdb_detach_tags(handle, alias, tags_to_detach, 2);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_detach_tags", qdb_error(error), error);
            }
            // doc-end-tag_detach

            error = qdb_attach_tag(handle, alias, tag);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_detach_tag", qdb_error(error), error);
            }

            // doc-start-tag_get
            const char ** aliases  = NULL;
            qdb_size_t alias_count = 0;

            error = qdb_get_tagged(handle, tag, &aliases, &alias_count);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_get_tagged", qdb_error(error), error);
            }

            qdb_release(handle, aliases);
            // doc-end-tag_get

            // doc-start-tag_iterate
            qdb_const_tag_iterator_t it;

            error = qdb_tag_iterator_begin(handle, tag, &it);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_tag_iterator_begin", qdb_error(error), error);
            }

            for (error = qdb_tag_iterator_begin(handle, tag, &it); error == qdb_e_ok; error = qdb_tag_iterator_next(&it))
            {
                // read content
                // it.alias
                // it.type
            }
            if (error && (error != qdb_e_iterator_end))
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_tag_iterator_next", qdb_error(error), error);
            }

            error = qdb_tag_iterator_close(&it);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_tag_iterator_close", qdb_error(error), error);
            }
            // doc-end-tag_iterate

            // doc-start-tag_meta
            error = qdb_has_tag(handle, alias, tag);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_has_tag", qdb_error(error), error);
            }

            const char ** entry_tags = NULL;
            qdb_size_t tag_count     = 0;

            error = qdb_get_tags(handle, alias, &entry_tags, &tag_count);
            if (error)
            {
                fprintf(stderr, "[%s] error: %s (%d)\n", "qdb_get_tags", qdb_error(error), error);
            }

            qdb_release(handle, entry_tags);
            // doc-end-tag_meta
        }

        qdb_close(handle);
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
