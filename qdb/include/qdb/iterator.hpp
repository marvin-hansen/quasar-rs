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

#include "buffer.hpp"
#include "iterator.h"
#include "tag.h"
#include <string>

namespace qdb
{

namespace detail
{

class const_iterator_impl
{
public:
    // might not look super efficient, but using raw pointers would result in
    // dangling pointers in too many scenarii
    // NOLINTNEXTLINE(modernize-use-using)
    typedef std::pair<std::string, api_buffer_ptr> value_type;

    struct init_reverse
    {
    };

    struct init_end
    {
    };

public:
    // initialize to begin
    explicit const_iterator_impl(qdb_handle_t h) : _handle(h)
    {
        // begin will properly initialize the iterator and return the first
        // found value
        _last_error = qdb_iterator_begin(_handle, &_iterator);
        _end = (_last_error != qdb_e_ok);
    }

    // initializes as reverse begin (final entry)
    const_iterator_impl(qdb_handle_t h, init_reverse /*unused*/) : _handle(h)
    {
        _last_error = qdb_iterator_rbegin(_handle, &_iterator);
        _end = (_last_error != qdb_e_ok);
    }

    // initializes as end iterator
    const_iterator_impl(qdb_handle_t h, init_end /*unused*/)
        : _handle(h), _end(true), _last_error(qdb_e_alias_not_found)
    {
        // makes sure the iterator will be seen as invalid to prevent any
        // strange error
        memset(&_iterator, 0, sizeof(_iterator));
        assert(end());
    }

public:
    friend void swap(const_iterator_impl & lhs,
                     const_iterator_impl & rhs) QDB_NOEXCEPT
    {
        using std::swap;

        swap(lhs._handle, rhs._handle);
        swap(lhs._end, rhs._end);
        swap(lhs._last_error, rhs._last_error);
        swap(lhs._value, rhs._value);
        swap(lhs._iterator, rhs._iterator);
    }

    // copy construction and assignment, we need to do a low level copy of the
    // iterator otherwise pointers will be dangling...
    const_iterator_impl(const const_iterator_impl & orig)
        : _handle(orig._handle), _end(orig._end), _last_error(orig._last_error),
          _value(orig._value)
    {
        qdb_iterator_copy(&orig._iterator, &_iterator);
    }

    const_iterator_impl & operator=(const_iterator_impl orig_copy)
    {
        close();

        // copy-and-swap idiom
        swap(orig_copy, *this);

        return *this;
    }

#ifdef QDBAPI_RVALUE_SUPPORT
public:
    const_iterator_impl(const_iterator_impl && orig) QDB_NOEXCEPT
        : const_iterator_impl(orig._handle)
    {
        // copy-and-swap idiom
        swap(orig, *this);
    }
#endif

public:
    ~const_iterator_impl()
    {
        close();
    }

public:
    void close()
    {
        // close is safe against double close, no need to check anything
        // we also made sure that all constructors will result in either a
        // cleared iterator or a valid one
        _last_error = qdb_iterator_close(&_iterator);
        _value = value_type();
    }

private:
    template <typename BeginFunction, typename NextFunction>
    const_iterator_impl & iterate(BeginFunction begin, NextFunction next)
    {
        if (valid() || (last_error() == qdb_e_alias_not_found))
        {
            _value = value_type();

            if (end())
            {
                // we want to allow this
                // auto it = h.rend();
                // --it;
                // because next will be called for -- in reverse iterators
                // of course
                // auto it = h.rbegin();
                // ----it;
                // will produce the same result, but it's supposed to be
                // undefined anyway
                //
                close();

                // the last entry when going backward is actually the first
                // entry (!)
                _last_error = begin(_handle, &_iterator);
            }
            else
            {
                _last_error = next(&_iterator);
            }
        }

        _end = (_last_error != qdb_e_ok);

        return *this;
    }

public:
    const_iterator_impl & next()
    {
        return iterate(&qdb_iterator_begin, &qdb_iterator_next);
    }

    const_iterator_impl & previous()
    {
        return iterate(&qdb_iterator_rbegin, &qdb_iterator_previous);
    }

public:
    bool operator==(const const_iterator_impl & it) const
    {
        // black magic, look somewhere else, thanks
        return (end() && it.end()) ||
               // both iterators need to be error free for the comparison to
               // make sense two invalid iterators are always considered unequal
               // even if they have the same error code (unless they are "end"
               // abstraction as checked above) for the simple reason it doesn't
               // make sense to compare iterators in error
               ((_last_error == qdb_e_ok) && (it._last_error == qdb_e_ok)
                && (_iterator.node && it._iterator.node
                    && !memcmp(_iterator.node, it._iterator.node, 32))
                && (_iterator.ref && it._iterator.ref
                    && !memcmp(_iterator.ref, it._iterator.ref, 32)));
    }

public:
    const value_type & value() const
    {
        // make sure the value is up to date
        value_ptr();

        // return a constant reference to it
        return _value;
    }

    value_type * value_ptr() const
    {
        // content already present or update doesn't make sense
        if (_value.second || end() || (_last_error != qdb_e_ok))
        {
            return &_value;
        }

        assert(_value.first.empty());
        assert(!_value.second);

        if (_iterator.content_size > 0)
        {
            void * local_copy = NULL; // NOLINT(modernize-use-nullptr)

            assert(_iterator.content);

            // there's no way around this copy, as soon as the user accesses the
            // value it may do whatever it wants with the buffer if we simply
            // point to the iterator buffer we will have a dangling pointer if
            // the iterator moves or is closed we use our high performance
            // allocator to reduce the cost of this cop

            _last_error = qdb_copy_alloc_buffer(_iterator.handle,
                                                _iterator.content,
                                                _iterator.content_size,
                                                &local_copy);
            if (_last_error != qdb_e_ok)
            {
                return &_value;
            }

            assert(local_copy);

            // NOLINTNEXTLINE(modernize-make-shared)
            _value.second.reset(new api_buffer(
                _iterator.handle, local_copy, _iterator.content_size));

            assert(_value.second->data());
        }
        else
        {
            _value.second.reset();
        }

        // alias is zero-terminated (server-side check)
        _value.first = _iterator.alias;

        assert(!_value.first.empty());

        return &_value;
    }

public:
    qdb_error_t last_error() const
    {
        return _last_error;
    }

public:
    bool end() const
    {
        return _end;
    }

    bool valid() const
    {
        return last_error() == qdb_e_ok;
    }

private:
    qdb_handle_t _handle;
    bool _end;
    qdb_const_iterator_t _iterator;

private:
    mutable qdb_error_t _last_error;
    // lazily updated
    mutable value_type _value;
};

class const_iterator_base
{
public:
    // NOLINTNEXTLINE(modernize-use-using)
    typedef std::bidirectional_iterator_tag iterator_category;
    // NOLINTNEXTLINE(modernize-use-using)
    typedef const_iterator_impl::value_type value_type;
    typedef std::ptrdiff_t difference_type; // NOLINT(modernize-use-using)
    typedef value_type * pointer;           // NOLINT(modernize-use-using)
    typedef value_type & reference;         // NOLINT(modernize-use-using)

protected:
    explicit const_iterator_base(qdb_handle_t h) : _impl(h) {}

    const_iterator_base(qdb_handle_t h, const_iterator_impl::init_reverse v)
        : _impl(h, v)
    {
    }

    // make end iterator
    const_iterator_base(qdb_handle_t h, const_iterator_impl::init_end v)
        : _impl(h, v)
    {
    }

protected:
    // we don't want reverse to compare with forward, we therefore make this
    // protected and use a name which is more convenient to explicitly call than
    // an operator
    bool equals(const const_iterator_base & it) const
    {
        return _impl == it._impl;
    }

protected:
    const_iterator_base & next()
    {
        _impl.next();
        return *this;
    }

    const_iterator_base & previous()
    {
        _impl.previous();
        return *this;
    }

public:
    // we return the alias/content pair, content will be 0 if we cannot access
    // it
    const value_type & operator*() const
    {
        return _impl.value();
    }

    value_type * operator->() const
    {
        return _impl.value_ptr();
    }

public:
    // needed by the Python API
    void close()
    {
        _impl.close();
    }

public:
    qdb_error_t last_error() const
    {
        return _impl.last_error();
    }

    bool valid() const
    {
        return _impl.valid();
    }

private:
    const_iterator_impl _impl;
};

} // namespace detail

class const_iterator : public detail::const_iterator_base
{
    // can only construct from handle class
    friend class handle;

private:
    explicit const_iterator(qdb_handle_t h) : detail::const_iterator_base(h) {}

    // make end iterator
    const_iterator(qdb_handle_t h, detail::const_iterator_impl::init_end v)
        : detail::const_iterator_base(h, v)
    {
    }

public:
    // if you compare with a reverse iterator, it will not compile, which is
    // what we want
    bool operator==(const const_iterator & other) const
    {
        return detail::const_iterator_base::equals(other);
    }

    bool operator!=(const const_iterator & other) const
    {
        return !(*this == other);
    }

public:
    const_iterator & operator++()
    {
        next();
        return *this;
    }

    const_iterator & operator--()
    {
        previous();
        return *this;
    }
};

class const_reverse_iterator : public detail::const_iterator_base
{
    // can only construct from handle class
    friend class handle;

private:
    explicit const_reverse_iterator(qdb_handle_t h)
        : const_iterator_base(h, detail::const_iterator_impl::init_reverse())
    {
    }

    // make rend iterator
    const_reverse_iterator(qdb_handle_t h,
                           detail::const_iterator_impl::init_end v)
        : const_iterator_base(h, v)
    {
    }

public:
    // if you compare with a forward iterator, it will not compile, which is
    // what we want
    bool operator==(const const_reverse_iterator & other) const
    {
        return detail::const_iterator_base::equals(other);
    }

    bool operator!=(const const_reverse_iterator & other) const
    {
        return !(*this == other);
    }

public:
    const_reverse_iterator & operator++()
    {
        previous();
        return *this;
    }

    const_reverse_iterator & operator--()
    {
        next();
        return *this;
    }
};

namespace detail
{

class const_tag_iterator_impl
{
public:
    // NOLINTNEXTLINE(modernize-use-using)
    typedef std::pair<std::string, qdb_entry_type_t> value_type;

    struct init_end
    {
    };

public:
    // initialize to begin
    const_tag_iterator_impl(qdb_handle_t h, const char * tag)
        : _handle(h), _tag(tag)
    {
        // begin will properly initialize the iterator and return the first
        // found value
        _last_error = qdb_tag_iterator_begin(_handle, _tag, &_iterator);
        _end = (_last_error != qdb_e_ok);
    }

    // initializes as end iterator
    const_tag_iterator_impl(qdb_handle_t h, init_end /*unused*/)
        : _handle(h), _end(true), _tag(NULL), // NOLINT(modernize-use-nullptr)
          _last_error(qdb_e_alias_not_found)
    {
        // makes sure the iterator will be seen as invalid to prevent any
        // strange error
        memset(&_iterator, 0, sizeof(_iterator));
        assert(end());
    }

public:
    friend void swap(const_tag_iterator_impl & lhs,
                     const_tag_iterator_impl & rhs)
    {
        using std::swap;

        swap(lhs._handle, rhs._handle);
        swap(lhs._end, rhs._end);
        swap(lhs._last_error, rhs._last_error);
        swap(lhs._tag, rhs._tag);
        swap(lhs._value, rhs._value);
        swap(lhs._iterator, rhs._iterator);
    }

    // copy construction and assignment, we need to do a low level copy of the
    // iterator otherwise pointers will be dangling...
    const_tag_iterator_impl(const const_tag_iterator_impl & orig)
        : _handle(orig._handle), _end(orig._end),
          _tag(NULL), // NOLINT(modernize-use-nullptr)
          _last_error(orig._last_error), _value(orig._value)
    {
        qdb_tag_iterator_copy(&orig._iterator, &_iterator);
    }

    const_tag_iterator_impl & operator=(const_tag_iterator_impl orig_copy)
    {
        close();

        // copy-and-swap idiom
        swap(orig_copy, *this);

        return *this;
    }

#ifdef QDBAPI_RVALUE_SUPPORT
public:
    const_tag_iterator_impl(const_tag_iterator_impl && orig) QDB_NOEXCEPT
        : const_tag_iterator_impl(orig._handle, orig._tag)
    {
        // copy-and-swap idiom
        swap(orig, *this);
    }
#endif

public:
    ~const_tag_iterator_impl()
    {
        close();
    }

public:
    void close()
    {
        // close is safe against double close, no need to check anything
        // we also made sure that all constructors will result in either a
        // cleared iterator or a valid one
        _last_error = qdb_tag_iterator_close(&_iterator);
        _value = value_type();
    }

private:
    template <typename BeginFunction, typename NextFunction>
    const_tag_iterator_impl & iterate(BeginFunction begin, NextFunction next)
    {
        if (valid() || (last_error() == qdb_e_alias_not_found))
        {
            _value = value_type();

            if (end())
            {
                // we want to allow this
                // auto it = h.rend();
                // --it;
                // because next will be called for -- in reverse iterators
                // of course
                // auto it = h.rbegin();
                // ----it;
                // will produce the same result, but it's supposed to be
                // undefined anyway
                //
                close();

                // the last entry when going backward is actually the first
                // entry (!)
                _last_error = begin(_handle, _tag, &_iterator);
            }
            else
            {
                _last_error = next(&_iterator);
            }
        }

        _end = (_last_error != qdb_e_ok);

        return *this;
    }

public:
    const_tag_iterator_impl & next()
    {
        return iterate(&qdb_tag_iterator_begin, &qdb_tag_iterator_next);
    }

public:
    bool operator==(const const_tag_iterator_impl & it) const
    {
        // black magic, look somewhere else, thanks
        return (end() && it.end()) ||
               // both iterators need to be error free for the comparison to
               // make sense two invalid iterators are always considered unequal
               // even if they have the same error code (unless they are "end"
               // abstraction as checked above) for the simple reason it doesn't
               // make sense to compare iterators in error
               ((_last_error == qdb_e_ok) && (it._last_error == qdb_e_ok)
                && (_iterator.handle == it._iterator.handle)
                && (_iterator.magic == it._iterator.magic)
                && (_iterator.token
                    == it._iterator.token)); // simplified comparison
    }

public:
    const value_type & value() const
    {
        // make sure the value is up to date
        value_ptr();

        // return a constant reference to it
        return _value;
    }

    value_type * value_ptr() const
    {
        // check if update doesn't make sense
        if (!_value.first.empty() || end() || (_last_error != qdb_e_ok))
        {
            return &_value;
        }

        assert(_value.first.empty());
        // assert(_value.second == qdb_entry_uninitialized);

        // alias is zero-terminated (server-side check)
        _value.first = _iterator.alias;
        _value.second = _iterator.type;

        assert(!_value.first.empty());
        assert(_value.second != qdb_entry_uninitialized);

        return &_value;
    }

public:
    qdb_error_t last_error() const
    {
        return _last_error;
    }

public:
    bool end() const
    {
        return _end;
    }

    bool valid() const
    {
        return last_error() == qdb_e_ok;
    }

private:
    qdb_handle_t _handle;
    bool _end;
    qdb_const_tag_iterator_t _iterator;
    const char * _tag;

private:
    mutable qdb_error_t _last_error;
    // lazily updated
    mutable value_type _value;
};

class const_tag_iterator_base
{
public:
    // NOLINTNEXTLINE(modernize-use-using)
    typedef std::forward_iterator_tag iterator_category;
    // NOLINTNEXTLINE(modernize-use-using)
    typedef const_tag_iterator_impl::value_type value_type;
    typedef std::ptrdiff_t difference_type; // NOLINT(modernize-use-using)
    typedef value_type * pointer;           // NOLINT(modernize-use-using)
    typedef value_type & reference;         // NOLINT(modernize-use-using)

protected:
    const_tag_iterator_base(qdb_handle_t h, const char * tag) : _impl(h, tag) {}

    // make end iterator
    const_tag_iterator_base(qdb_handle_t h, const_tag_iterator_impl::init_end v)
        : _impl(h, v)
    {
    }

protected:
    // we don't want reverse to compare with forward, we therefore make this
    // protected and use a name which is more convenient to explicitly call than
    // an operator
    bool equals(const const_tag_iterator_base & it) const
    {
        return _impl == it._impl;
    }

protected:
    const_tag_iterator_base & next()
    {
        _impl.next();
        return *this;
    }

public:
    const value_type & operator*() const
    {
        return _impl.value();
    }

    value_type * operator->() const
    {
        return _impl.value_ptr();
    }

public:
    // needed by the Python API
    void close()
    {
        _impl.close();
    }

public:
    qdb_error_t last_error() const
    {
        return _impl.last_error();
    }

    bool valid() const
    {
        return _impl.valid();
    }

private:
    const_tag_iterator_impl _impl;
};

} // namespace detail

class const_tag_iterator : public detail::const_tag_iterator_base
{
    // can only construct from handle class
    friend class handle;

private:
    const_tag_iterator(qdb_handle_t h, const char * tag)
        : detail::const_tag_iterator_base(h, tag)
    {
    }
    // make end iterator
    const_tag_iterator(qdb_handle_t h,
                       detail::const_tag_iterator_impl::init_end v)
        : detail::const_tag_iterator_base(h, v)
    {
    }

public:
    bool operator==(const const_tag_iterator & other) const
    {
        return detail::const_tag_iterator_base::equals(other);
    }

    bool operator!=(const const_tag_iterator & other) const
    {
        return !(*this == other);
    }

public:
    const_tag_iterator & operator++()
    {
        next();
        return *this;
    }
};

} // namespace qdb
