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

#include <qdb/blob.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>

void print_error(qdb_error_t err)
{
    std::cerr << "Error: " << qdb_error(err) << '\n';
}

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <uri> <alias>\n";
        std::cerr << "Example: " << argv[0] << " qdb://127.0.0.1:2836 alias\n";
        return EXIT_FAILURE;
    }

    qdb::handle handle;
    qdb_error_t error = handle.connect(argv[1]);
    if (error)
    {
        print_error(error);
        return EXIT_FAILURE;
    }

    qdb::api_buffer_ptr content = handle.blob_get(argv[2], error);
    if (error)
    {
        print_error(error);
        return EXIT_FAILURE;
    }
    if (!content)
    {
        std::cerr << "Error: null content" << '\n';
        return EXIT_FAILURE;
    }

    std::fwrite(content->data(), content->size(), 1u, stdout);

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
