#pragma once

/*
 *
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

#include <cassert> // assert
#include <cstring> // memcmp
#include <memory>  // shared_ptr
#if defined(QDB_SHARED_PTR_USE_TR1)
#    include <tr1/memory> // tr1::shared_ptr
#endif

#ifndef QDB_SHARED_PTR_USE_TR1
#    ifdef _MSC_VER
#        if _MSC_VER < 1600
// before Visual Studio 2012, shared_ptr is in TR1
#            define QDB_SHARED_PTR_USE_TR1 1
#        endif // _MSC_VER < 1600
#    endif     // _MSC_VER
#endif         // QDB_SHARED_PTR_USE_TR1

namespace qdb
{

#if defined(QDB_SHARED_PTR_USE_TR1) && !defined(_LIBCPP_VERSION)

// libc++ does not have TR1 extensions.

template <typename T>
struct shared_ptr
{
    typedef std::tr1::shared_ptr<T> type; // NOLINT(modernize-use-using)
};

#else

template <typename T>
struct shared_ptr
{
    typedef std::shared_ptr<T> type; // NOLINT(modernize-use-using)
};

#endif

class api_buffer
{
public:
    api_buffer(qdb_handle_t h, const void * data, qdb_size_t length)
        : _handle(h), _data(static_cast<const char *>(data)), _length(length)
    {
        assert(_handle);
        assert(_data);
        assert(_length > 0);
    }

    ~api_buffer()
    {
        qdb_release(_handle, _data);
    }

private:
    // prevent copy
    api_buffer(const api_buffer & /*unused*/)
        : _handle(NULL), // NOLINT(modernize-use-nullptr)
          _data(NULL),   // NOLINT(modernize-use-nullptr)
          _length(0)
    {
    }

public:
    bool operator==(const api_buffer & other) const
    {
        return (size() == other.size())
               && !memcmp(data(), other.data(), size());
    }

    bool operator!=(const api_buffer & other) const
    {
        return !(*this == other);
    }

public:
    const char * data() const // NOLINT(modernize-use-nodiscard)
    {
        return _data;
    }

    qdb_size_t size() const // NOLINT(modernize-use-nodiscard)
    {
        return _length;
    }

    const char * begin() const // NOLINT(modernize-use-nodiscard)
    {
        return data();
    }

    const char * end() const // NOLINT(modernize-use-nodiscard)
    {
        return data() + size();
    }

private:
    const qdb_handle_t _handle;
    const char * const _data;
    const qdb_size_t _length;
};

// NOLINTNEXTLINE(modernize-use-using)
typedef qdb::shared_ptr<api_buffer>::type api_buffer_ptr;

inline api_buffer_ptr
make_api_buffer_ptr(qdb_handle_t h, const void * data, qdb_size_t length)
{
    if (!data || !length)
    {
        // NOLINTNEXTLINE(modernize-return-braced-init-list)
        return api_buffer_ptr();
    }

#ifdef QDB_SHARED_PTR_USE_TR1
    return api_buffer_ptr(new api_buffer(h, data, length));
#else
    return std::make_shared<api_buffer>(h, data, length);
#endif
}

} // namespace qdb
