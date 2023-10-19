#pragma once

/*
 *
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

#include "batch.h"
#include "blob.h"
#include "client.h"
#include "error.hpp"
#include "integer.h"
#include "iterator.hpp"
#include "node.h"
#include "option.h"
#include "prefix.h"
#include "suffix.h"
#include "tag.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <vector>

namespace qdb
{

static const qdb_time_t never_expires = qdb_never_expires;
static const qdb_time_t preserve_expiration = qdb_preserve_expiration;

inline std::string make_error_string(qdb_error_t error)
{
    return std::string(qdb_error(error));
}

struct remote_node
{
public:
    remote_node() {}

    remote_node(std::string address, unsigned short port)
        : _address(address), _port(port), _error(qdb_e_ok)
    {
    }

    std::string & address()
    {
        return _address;
    }
    const std::string & address() const
    {
        return _address;
    }

    unsigned short & port()
    {
        return _port;
    }
    unsigned short port() const
    {
        return _port;
    }

    qdb_error_t & error()
    {
        return _error;
    }
    qdb_error_t error() const
    {
        return _error;
    }

private:
    std::string _address;
    unsigned short _port;
    qdb_error_t _error;
};

class handle
{
public:
    // you can't connect in the constructor as you would be unable to know if
    // the connection was successful
    // we don't want to encourage exceptions usage in our C++ API and therefore
    // provide the class with a separate
    // connect() method
    handle() : _handle(NULL), _timeout(60 * 1000), _encrypt(qdb_crypt_none) {}
    ~handle()
    {
        close();
    }

#ifdef QDBAPI_RVALUE_SUPPORT
public:
    handle(handle && h) : _handle(0), _timeout(h._timeout)
    {
        using std::swap;
        swap(h._handle, _handle);
    }

    handle & operator=(handle && h)
    {
        _timeout = h._timeout;
        using std::swap;
        swap(h._handle, _handle);
        return *this;
    }
#endif

private:
    // prevent copy, we might get a dangling handle otherwise
    // use a pointer or a smart pointer instead of copying objects around
    handle(const handle & /*unused*/) : _handle(NULL), _timeout(60 * 1000) {}

public:
    //! \ingroup client
    //!
    //! \brief STLesque iterators with a twist,
    // you can compare with end, but also make sure the iterator is valid at
    // each step if the iterator is invalid, you should check if it's because
    // you reached the end or because of some other error (see last_error())
    // also, when the iterator reaches end() or rend() going forward or backward
    // is currently unsupported this is due to how we implemented end() and
    // rend() iterator values
    //!
    //! \return A forward iterator pointing to the first entry in the cluster.
    const_iterator begin()
    {
        return const_iterator(this->_handle);
    }
    //! \ingroup client
    //!
    //! \brief End iterator
    //!
    //! \return A forward iterator pointing beyond the last entry in the
    //! cluster.
    const_iterator end()
    {
        return const_iterator(this->_handle,
                              detail::const_iterator_impl::init_end());
    }
    //! \ingroup client
    //!
    //! \brief Constant reverse begin iterator
    //!
    //! \return A reverse iterator pointing to the last entry in the cluster.
    const_reverse_iterator rbegin()
    {
        return const_reverse_iterator(this->_handle);
    }
    //! \ingroup client
    //!
    //! \brief Constant reverse end iterator
    //!
    //! \return A reverse iterator pointing before the first entry in the
    //! cluster.
    const_reverse_iterator rend()
    {
        return const_reverse_iterator(this->_handle,
                                      detail::const_iterator_impl::init_end());
    }

public:
    //! \ingroup client
    //!
    //! \brief Closes the handle and release all associated resources.
    //!
    //! \warning Not thread-safe
    void close()
    {
        if (connected())
        {
            // although close returns an error, we can safely ignore it as we
            // know for sure the handle
            // we pass is an unclosed handle
            qdb_close(_handle);
            _handle = NULL;
        }
        assert(!connected());
    }

public:
    //! \ingroup client
    //!
    //! \brief  Determines if the handle is connected or not.
    //!
    //! \return True if the handle is connected, false otherwise
    bool connected() const
    {
        return _handle != NULL;
    }

public:
    // the user may want to access the handle, make that convenient
    operator qdb_handle_t()
    {
        return _handle;
    }

    operator bool() const
    {
        return _handle != NULL;
    }

    bool operator!() const
    {
        return _handle == NULL;
    }

public:
    qdb_error_t set_encryption(qdb_encryption_t crypt)
    {
        qdb_error_t err = qdb_e_ok;

        _encrypt = crypt;

        if (_handle)
        {
            err = qdb_option_set_encryption(_handle, _encrypt);
        }

        return err;
    }

    qdb_error_t set_cluster_public_key(const std::string & cluster_pk)
    {
        qdb_error_t err = qdb_e_ok;

        _cluster_pk = cluster_pk;

        if (_handle)
        {
            err =
                qdb_option_set_cluster_public_key(_handle, _cluster_pk.c_str());
        }

        return err;
    }

    qdb_error_t set_user_credentials(const std::string & user_id,
                                     const std::string & user_sk)
    {
        qdb_error_t err = qdb_e_ok;

        _user_id = user_id;
        _user_sk = user_sk;

        if (_handle)
        {
            err = qdb_option_set_user_credentials(
                _handle, user_id.c_str(), user_sk.c_str());
        }

        return err;
    }

    qdb_error_t cluster_reload_user_config()
    {
        qdb_error_t err = qdb_e_ok;
        if (_handle)
        {
            err = qdb_cluster_reload_user_config(_handle);
        }

        return err;
    }

public:
    //! \ingroup client
    //!
    //! \brief Sets the timeout, for all operations.
    //!
    //! \param timeout_ms The timeout in milliseconds
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t set_timeout(int timeout_ms)
    {
        qdb_error_t err = qdb_e_ok;

        _timeout = timeout_ms;

        if (_handle)
        {
            err = qdb_option_set_timeout(_handle, _timeout);
        }

        return err;
    }

    qdb_error_t set_client_max_in_buf_size(size_t max_size)
    {
        return qdb_option_set_client_max_in_buf_size(_handle, max_size);
    }

    qdb_error_t set_stabilization_max_wait(int max_wait)
    {
        return qdb_option_set_stabilization_max_wait(_handle, max_wait);
    }

    qdb_error_t set_transaction_max_wait(int max_wait)
    {
        return qdb_option_set_transaction_max_wait(_handle, max_wait);
    }

    qdb_error_t set_max_cardinality(qdb_uint_t max_cardinality)
    {
        return qdb_option_set_max_cardinality(_handle, max_cardinality);
    }

    //! \ingroup client
    //!
    //! \brief Sets the compression level for all network calls.
    //!
    //! \param comp_level The compression level.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t set_compression(qdb_compression_t comp_level)
    {
        return qdb_option_set_compression(_handle, comp_level);
    }

public:
    // convenience method
    qdb_error_t connect(const std::string & uri)
    {
        return connect(uri.c_str());
    }

    //! \ingroup client
    //!
    //! \brief Initializes all required resources and connect to a remote host.
    //!
    //! Connects to the remote host in TCP if a connection is already opened, it
    //! will be closed the internal handle will be updated if and only if the
    //! connection is successful
    //!
    //! \param uri A pointer to a null-terminated string in the format
    //! qdb://host:port[,host:port].
    //!
    //! \return A qdb_error_t code indicating success or failure.
    //!
    //! \remark Not thread-safe
    qdb_error_t connect(const char * uri)
    {
        close();

        if (!uri)
        {
            return qdb_e_invalid_argument;
        }

        qdb_handle_t h;
        qdb_error_t err = qdb_open(&h, qdb_p_tcp);

        if (err != qdb_e_ok)
        {
            return err;
        }

        assert(h);

        err = qdb_option_set_timeout(h, _timeout);
        if (err != qdb_e_ok)
        {
            qdb_close(h);
            return err;
        }

        err = qdb_option_set_encryption(h, _encrypt);
        if (err != qdb_e_ok)
        {
            qdb_close(h);
            return err;
        }

        if (!_cluster_pk.empty() && !_user_id.empty() && !_user_sk.empty())
        {
            err = qdb_option_set_cluster_public_key(h, _cluster_pk.c_str());
            if (err != qdb_e_ok)
            {
                qdb_close(h);
                return err;
            }

            err = qdb_option_set_user_credentials(
                h, _user_id.c_str(), _user_sk.c_str());
            if (err != qdb_e_ok)
            {
                qdb_close(h);
                return err;
            }
        }

        err = qdb_connect(h, uri);
        if (err != qdb_e_ok)
        {
            qdb_close(h);
        }
        else
        {
            _handle = h;
        }

        return err;
    }

public:
    //! \ingroup client
    //!
    //! \brief Adds an entry to the quasardb server. If the entry already exists
    //! the method will fail and will return qdb_e_alias_already_exists.
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //! entry's alias to create.
    //!
    //! \param content A pointer to a buffer that represents the entry's
    //! content to be added to the server.
    //!
    //! \param content_length The length of the entry's content, in bytes.
    //!
    //! \param expiry_time The absolute expiry
    //! time of the entry, in seconds, relative to epoch
    //!
    //!\return A qdb_error_t code indicating success or failure.
    qdb_error_t blob_put(const char * alias,
                         const void * content,
                         qdb_size_t content_length,
                         qdb_time_t expiry_time)
    {
        return qdb_blob_put(
            _handle, alias, content, content_length, expiry_time);
    }

public:
    // integer
    //! \ingroup client
    //!
    //! \brief Retrieves the value of an integer. The integer must already
    //! exist.
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //! entry's alias.
    //!
    //! \param number The value of the retrieved qdb_int_t.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t int_get(const char * alias, qdb_int_t * number)
    {
        return qdb_int_get(_handle, alias, number);
    }
    //! \ingroup client
    //!
    //! \brief  Creates a new integer. Errors if the integer already exists.
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias.
    //!
    //! \param number The value of the retrieved qdb_int_t.
    //!
    //! \param expiry_time The absolute expiry time of the entry, in seconds,
    //! relative to epoch
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t
    int_put(const char * alias, qdb_int_t number, qdb_time_t expiry_time)
    {
        return qdb_int_put(_handle, alias, number, expiry_time);
    }
    //! \ingroup client
    //!
    //! \brief  Updates an existing integer or creates one if it does not exist.
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias.
    //!
    //! \param number The value of the retrieved qdb_int_t.
    //!
    //! \param expiry_time The absolute expiry time of the entry, in seconds,
    //! relative to epoch
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t
    int_update(const char * alias, qdb_int_t number, qdb_time_t expiry_time)
    {
        return qdb_int_update(_handle, alias, number, expiry_time);
    }
    //! \ingroup client
    //!
    //! \brief Atomically addes the value to the integer. The integer must
    //! already exist.
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //!  entry's alias.
    //!
    //! \param addend The value that will be added to the existing qdb_int_t.
    //!
    //! \param result A pointer that will be updated to point to the new
    //! qdb_int_t.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t
    int_add(const char * alias, qdb_int_t addend, qdb_int_t * result = NULL)
    {
        return qdb_int_add(_handle, alias, addend, result);
    }

public:
    //! \ingroup client
    //!
    //! \brief Updates an entry on the quasardb server.
    //!
    //! If the entry already exists, the content will be updated. If the entry
    //! does not exist, it will be created. The handle must be initialized and
    //! connected (see :func:`connect`).
    //!
    //! \param alias: A pointer to a null-terminated string representing the
    //! entry's alias to update.
    //!
    //! \param content: A pointer to a buffer buffer that represents the entry's
    //! content to be updated to the server.
    //! \param content_length: The length of the entry's content, in bytes.
    //!
    //! \param expiry_time The absolute expiry time of the entry, in seconds,
    //! relative to epoch
    //!
    //! \return A qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is experimental.
    qdb_error_t blob_update(const char * alias,
                            const void * content,
                            qdb_size_t content_length,
                            qdb_time_t expiry_time)
    {
        return qdb_blob_update(
            _handle, alias, content, content_length, expiry_time);
    }

public:
    //! \ingroup client
    //!
    //! \brief Retrieves an entry's content from the quasardb server. The caller
    //! is responsible for allocating and freeing the provided buffer.
    //!
    //! If the entry does not exist, the method will fail and return
    //! qdb_e_alias_not_found
    //!
    //! If the buffer is not large enough to hold the data, the function will
    //! fail and return qdb_e_buffer_too_small. content_length will
    //! nevertheless be updated with entry size so that the caller may resize
    //! its buffer and try again.
    //!
    //! The handle must be initialized and connected (see :func:`connect`).
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //! entry's alias to be retrieved.
    //!
    //! \param content A pointer to an user allocated buffer that will receive
    //! the entry's content.
    //!
    //! \param content_length A pointer to a qdb_size_t initialized with the
    //! length of the destination buffer, in bytes. It will be updated with the
    //! length of the retrieved content, even if the buffer is not large enough
    //! to hold all the data.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t blob_get_noalloc(const char * alias,
                                 void * content,
                                 qdb_size_t * content_length)
    {
        return qdb_blob_get_noalloc(_handle, alias, content, content_length);
    }

public:
    //! \ingroup client
    //!
    //! \brief Runs the provided operations in batch on the cluster. The
    //! operations are run in arbitrary order.
    //!
    //! It is preferred to use the std::vector version of run_batch where
    //! possible.
    //!
    //! \param operations Pointer to an array of qdb_operation_t
    //!
    //! \param operation_count Size of the array, in entry count
    //!
    //! \return The number of successful operations
    size_t run_batch(qdb_operation_t * operations, size_t operation_count)
    {
        return qdb_run_batch(_handle, operations, operation_count);
    }

public:
    //! \ingroup client
    //!
    //! \brief Runs the provided operations as a transaction on the cluster. The
    //! operations are run in the provided order. If any operation fails, all
    //! previously run operations are rolled back.
    //!
    //! \param operations: Pointer to an array of qdb_operation_t
    //!
    //! \param operation_count Size of the array, in entry count
    //!
    //! \param fail_index: The index in the operations array for the operation
    //! that failed.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t run_transaction(qdb_operation_t * operations,
                                size_t operation_count,
                                size_t & fail_index)
    {
        return qdb_run_transaction(
            _handle, operations, operation_count, &fail_index);
    }

private:
    api_buffer_ptr translate_result_buffer(qdb_error_t error,
                                           const void * content,
                                           qdb_size_t content_length) const
    {
        return (error == qdb_e_ok)
                   ? make_api_buffer_ptr(_handle, content, content_length)
                   : api_buffer_ptr();
    }

public:
    //! \ingroup client
    //!
    //! \brief Retrieves an entry's content from the quasardb server.
    //!
    //! If the entry does not exist, the function will fail and update error to
    //! qdb_e_alias_not_found
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content.
    //!
    //! The handle must be initialized and connected (see connect).
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //! entry's alias whose content is to be retrieved.
    //!
    //! \param error A reference to an error that will receive the result of
    //! the operation.
    //!
    //! \return  An api_buffer_ptr holding the entry content, if it exists, a
    //! null pointer otherwise.
    api_buffer_ptr blob_get(const char * alias, qdb_error_t & error)
    {
        const void * content = NULL;
        qdb_size_t content_length = 0;

        error = qdb_blob_get(_handle, alias, &content, &content_length);

        return translate_result_buffer(error, content, content_length);
    }
    //! \ingroup client
    //!
    //! \brief Atomically gets an entry from the quasardb server and removes it.
    //! If the entry does not exist, the function will fail and update error to
    //! qdb_e_alias_not_found.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content.
    //!
    //! The handle must be initialized and connected (see connect).
    //!
    //! \param alias A pointer to a null-terminated string representing the
    //! entry's alias whose content is to be retrieved.
    //!
    //! \param error A reference to an error that will receive the result of
    //! the operation.
    //!
    //! \return An api_buffer_ptr holding the entry content, if it exists, a
    //! null pointer otherwise.
    api_buffer_ptr blob_get_and_remove(const char * alias, qdb_error_t & error)
    {
        const void * content = NULL;
        qdb_size_t content_length = 0;

        error =
            qdb_blob_get_and_remove(_handle, alias, &content, &content_length);

        return translate_result_buffer(error, content, content_length);
    }
    //! \ingroup client
    //!
    //! \brief Atomically gets and updates(in this order) the entry on the
    //! quasardb server.The entry must already exist.
    //!
    //! The handle must be initialized and connected(see :func:`connect`).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //!     entry's alias to update.
    //!
    //! \param update_content  A pointer to a buffer that represents the entry's
    //! content to be updated to the server.
    //!
    //! \param update_content_length  The length of the buffer, in bytes.
    //!
    //! \param expiry_time  The absolute expiry time of the entry, in seconds,
    //! relative to epoch
    //!
    //! \param error  A reference to an error that will receive the result of
    //! the operation.
    //!
    //! \return An api_buffer_ptr holding the entry content, if it exists, a
    //! null pointer otherwise.
    api_buffer_ptr blob_get_and_update(const char * alias,
                                       const void * update_content,
                                       qdb_size_t update_content_length,
                                       qdb_time_t expiry_time,
                                       qdb_error_t & error)
    {
        const void * content = NULL;
        qdb_size_t content_length = 0;

        error = qdb_blob_get_and_update(_handle,
                                        alias,
                                        update_content,
                                        update_content_length,
                                        expiry_time,
                                        &content,
                                        &content_length);

        return translate_result_buffer(error, content, content_length);
    }

    //! \ingroup client
    //!
    //! \brief Atomically compares the entry with comparand and updates it to
    //! new_value if, and only if, they match.Always return the original value
    //! of the entry.
    //!
    //! The handle must be initialized and connected(see :func:`connect`).
    //!
    //! \param alias A pointer to a null - terminated string
    //! representing the entry's alias to compare to.
    //!
    //! \param new_value A pointer to a buffer that represents the entry's
    //! content to be updated to the server in case of match.
    //!
    //! \param new_value_length The length of the buffer, in bytes.
    //!
    //! \param comparand A pointer to a buffer that represents the entry's
    //! content to be compared to.
    //!
    //! \param comparand_length The length of the buffer, in bytes.
    //!
    //! \param expiry_time  The absolute expiry time of the entry, in seconds,
    //! relative to epoch
    //!
    //! \param error A reference to an error that will receive the result of
    //! the operation.
    //!
    //! \return An api_buffer_ptr holding the entry content, if it
    //! exists, a null pointer otherwise.
    api_buffer_ptr blob_compare_and_swap(const char * alias,
                                         const void * new_value,
                                         qdb_size_t new_value_length,
                                         const void * comparand,
                                         qdb_size_t comparand_length,
                                         qdb_time_t expiry_time,
                                         qdb_error_t & error)
    {
        const void * content = NULL;
        qdb_size_t content_length = 0;

        error = qdb_blob_compare_and_swap(_handle,
                                          alias,
                                          new_value,
                                          new_value_length,
                                          comparand,
                                          comparand_length,
                                          expiry_time,
                                          &content,
                                          &content_length);

        // make buffer even on error
        return make_api_buffer_ptr(_handle, content, content_length);
    }

public:
    //! \ingroup client
    //!
    //! \brief Sets the expiry time of an existing entry from the quasardb
    //! cluster.A value of zero means the entry never expires.
    //!
    //! The handle must be initialized and connected(see connect).
    //!
    //! \param alias  A pointer to a null - terminated string representing the
    //! entry's alias for which the expiry must be set.
    //!
    //! \param expiry_time  Absolute time after which the entry expires
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t expires_at(const char * alias, qdb_time_t expiry_time)
    {
        return qdb_expires_at(_handle, alias, expiry_time);
    }
    //! \ingroup client
    //!
    //! \brief Sets the expiry time of an existing entry from the quasardb
    //! cluster. A value of zero means the entry expires as soon as possible.
    //!
    //! The handle must be initialized and connected(see :func:`connect`).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias for which the expiry must be set.
    //!
    //! \param expiry_delta Time in seconds, relative to the call time, after
    //! which the entry expires
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t expires_from_now(const char * alias, qdb_time_t expiry_delta)
    {
        return qdb_expires_from_now(_handle, alias, expiry_delta);
    }

    //! \ingroup client
    //!
    //! \brief Retrieves the expiry time of an existing entry. A value of zero
    //! means the entry never expires.
    //!
    //! The handle must be initialized and connected(see connect).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias for which the expiry must be get.
    //!
    //! \param expiry_time : A pointer to a qdb_time_t that will receive the
    //! absolute expiry time.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t get_expiry_time(const char * alias, qdb_time_t & expiry_time)
    {
        qdb_entry_metadata_t meta;

        meta.expiry_time.tv_sec = 0;
        meta.expiry_time.tv_nsec = 0;

        qdb_error_t err = qdb_get_metadata(_handle, alias, &meta);

        expiry_time =
            static_cast<qdb_time_t>(meta.expiry_time.tv_sec) * 1000
            + static_cast<qdb_time_t>(meta.expiry_time.tv_nsec) / 1000000;

        return err;
    }
    //! \ingroup client
    //!
    //! \brief  Retrieves an array of locations where the entry is stored in the
    //! cluster.
    //!
    //! The handle must be initialized(see qdb_open and qdb_open_tcp) and the
    //! connection established(see qdb_connect).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias.
    //!
    //! \param location A pointer to a qdb_remote_node_t that will receive the
    //! entry locations.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t get_location(const char * alias, remote_node & location)
    {
        qdb_remote_node_t loc;
        qdb_error_t err = qdb_get_location(_handle, alias, &loc);
        if (err != qdb_e_ok)
        {
            return err;
        }
        location = remote_node(loc.address, loc.port);
        qdb_release(_handle, &loc);
        return err;
    }
    //! \ingroup client
    //!
    //! \brief Retrieves the type of the entry.
    //!
    //! The handle must be initialized(see qdb_open and qdb_open_tcp) and the
    //! connection established(see qdb_connect).
    //!
    //!  \param alias  A pointer to a null - terminated string representing the
    //!  entry's alias.
    //!
    //!  \param entry_metadata : A pointer to a qdb_entry_metadata_t that will
    //!  receive the entry locations.
    //!
    //!  \return A qdb_error_t code indicating success or failure.
    qdb_error_t get_metadata(const char * alias,
                             qdb_entry_metadata_t * entry_metadata)
    {
        return qdb_get_metadata(_handle, alias, entry_metadata);
    }

    QDB_DEPRECATED("use get_metadata")
    qdb_error_t get_type(const char * alias, qdb_entry_type_t * entry_type)
    {
        if (!entry_type) return qdb_e_invalid_argument;

        qdb_entry_metadata_t meta;
        meta.type = qdb_entry_uninitialized;
        qdb_error_t err = qdb_get_metadata(_handle, alias, &meta);
        *entry_type = meta.type;
        return err;
    }

private:
    template <typename F>
    std::string node_json(const char * uri, qdb_error_t & error, F f)
    {
        const char * content = NULL;
        qdb_size_t content_length = 0;
        error = f(_handle, uri, &content, &content_length);
        std::string result;
        if (error == qdb_e_ok)
        {
            result = std::string(content, content_length);
            qdb_release(_handle, content);
        }
        return result;
    }

public:
    //! \ingroup client
    //!
    //! \brief Obtains a node status as a JSON string.
    //!
    //! The handle must be initialized and connected(see : func : `connect`).
    //!
    //! \param uri A pointer to a null - terminated string in the format
    //!  qdb://host:port
    //!
    //! \param error A reference to an error code that will be updated
    //! according to the success of the operation
    //!
    //! \return The status of the node as a JSON string.
    std::string node_status(const char * uri, qdb_error_t & error)
    {
        return node_json(uri, error, &qdb_node_status);
    }

    //! \ingroup client
    //!
    //! \brief Obtains a node configuration as a JSON string.
    //!
    //! The handle must be initialized and connected(see : func : `connect`).
    //!
    //! \param uri A pointer to a null - terminated string in the format
    //!  qdb://host:port
    //!
    //! \param error A reference to an error code that will be updated
    //! according to the success of the operation
    //!
    //! \return The configuration of the node as a JSON string.
    std::string node_config(const char * uri, qdb_error_t & error)
    {
        return node_json(uri, error, &qdb_node_config);
    }

    //! \ingroup client
    //!
    //! \brief Obtains a node topology as a JSON string.
    //!
    //! The handle must be initialized and connected(see : func : `connect`).
    //!
    //! \param uri A pointer to a null - terminated string in the format
    //!  qdb://host:port
    //!
    //! \param error A reference to an error code that will be updated
    //! according to the success of the operation
    //!
    //! \return The topology of the node as a JSON string.
    std::string node_topology(const char * uri, qdb_error_t & error)
    {
        return node_json(uri, error, &qdb_node_topology);
    }

public:
    //! \ingroup client
    //!
    //! \brief Stops the node designated by its host and port number. This stop
    //! is generally effective a couple of seconds after it has been issued,
    //! enabling inflight calls to complete successfully.
    //!
    //! The handle must be initialized and connected(see :func:`connect`).
    //!
    //! \param uri A pointer to a null - terminated string in the format
    //! qdb://host:port.
    //!
    //! \param reason A pointer to a null - terminated string detailling the
    //! reason for the stop that will appear in the remote node's log.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is meant for very specific use cases and its
    //! usage is discouraged.
    qdb_error_t node_stop(const char * uri, const char * reason)
    {
        return qdb_node_stop(_handle, uri, reason);
    }

public:
    //! \ingroup client
    //!
    //! \brief Removes an entry from the quasardb server. If the entry does not
    //! exist, the function will fail and return qdb_e_alias_not_found
    //!
    //! The handle must be initialized and connected(see connect).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias to delete.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t remove(const char * alias)
    {
        return qdb_remove(_handle, alias);
    }

    //! \ingroup client
    //!
    //! \brief Removes an entry from the quasardb server if it matches
    //! comparand. The operation is atomic. If the entry does not exist, the
    //! function will fail and return ``qdb_e_alias_not_found``.
    //!
    //! The handle must be initialized and connected(see :func:`connect`).
    //!
    //! \param alias A pointer to a null - terminated string representing the
    //! entry's alias to delete.
    //!
    //! \param comparand A pointer to a buffer that represents the entry's
    //! content to be compared to.
    //!
    //! \param comparand_length The length of the buffer, in bytes.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t blob_remove_if(const char * alias,
                               const void * comparand,
                               qdb_size_t comparand_length)
    {
        return qdb_blob_remove_if(_handle, alias, comparand, comparand_length);
    }

private:
    struct blob_scan_binder
    {
        blob_scan_binder(const void * pattern,
                         qdb_size_t pattern_length,
                         qdb_int_t max_count)
            : _pattern(pattern), _pattern_length(pattern_length),
              _max_count(max_count)
        {
        }

        qdb_error_t operator()(qdb_handle_t handle,
                               const char *** results,
                               size_t * result_count)
        {
            return qdb_blob_scan(handle,
                                 _pattern,
                                 _pattern_length,
                                 _max_count,
                                 results,
                                 result_count);
        }

    private:
        const void * _pattern;
        qdb_size_t _pattern_length;
        qdb_int_t _max_count;
    };

public:
    //! \ingroup client
    //!
    //! \brief Scans blobs for a given pattern
    //!
    //! \param pattern A const void pointer representing the string to scan
    //!
    //! \param pattern_length The length of the pattern
    //!
    //! \param max_count The maximum number of results to fetch
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A vector of size atmost pattern_length of blob aliases matching
    //! the pattern
    //!
    //! \warning Experimental function.
    std::vector<std::string> blob_scan(const void * pattern,
                                       qdb_size_t pattern_length,
                                       qdb_int_t max_count,
                                       qdb_error_t & error)
    {
        return get_alias_list(
            error, blob_scan_binder(pattern, pattern_length, max_count));
    }

private:
    struct blob_scan_regex_binder
    {
        blob_scan_regex_binder(const char * pattern, qdb_int_t max_count)
            : _pattern(pattern), _max_count(max_count)
        {
        }

        qdb_error_t operator()(qdb_handle_t handle,

                               const char *** results,
                               size_t * result_count)
        {
            return qdb_blob_scan_regex(
                handle, _pattern, _max_count, results, result_count);
        }

    private:
        const char * _pattern;
        qdb_int_t _max_count;
    };

public:
    //! \ingroup client
    //!
    //! \brief Scans blobs for a given regex pattern
    //!
    //! \param pattern A const void pointer representing the string to scan
    //!
    //! \param max_count The maximum number of results to fetch
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A vector of size atmost pattern_length of blob aliases matching
    //! the pattern
    //!
    //! \warning Experimental function.
    std::vector<std::string> blob_scan_regex(const char * pattern,
                                             qdb_int_t max_count,
                                             qdb_error_t & error)
    {
        return get_alias_list(error,
                              blob_scan_regex_binder(pattern, max_count));
    }

private:
    typedef qdb_error_t (*affix_get)(qdb_handle_t,
                                     const char *,
                                     qdb_int_t,
                                     const char ***,
                                     size_t *);

    struct affix_binder
    {
        affix_binder(affix_get func, const char * affix, qdb_int_t max_count)
            : _function(func), _affix(affix), _max_count(max_count)
        {
        }

        qdb_error_t operator()(qdb_handle_t handle,
                               const char *** results,
                               size_t * result_count)
        {
            return _function(handle, _affix, _max_count, results, result_count);
        }

    private:
        affix_get _function;
        const char * _affix;
        qdb_int_t _max_count;
    };

public:
    //! \brief Retrieves the list of all entries matching the provided prefix.
    //!
    //! A prefix-based search will enable you to find all entries matching a
    //! provided \p prefix.
    //!
    //! This function return the list of aliases. It's up to the user to query
    //! the content associated with every entry, if needed.
    //!
    //! \param prefix A pointer to a null-terminated UTF-8 string representing
    //! the desired prefix
    //!
    //! \param max_count An integer limiting the number of results returned by
    //! the function
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A vector of string aliases matching the prefix pattern
    std::vector<std::string>
    prefix_get(const char * prefix, qdb_int_t max_count, qdb_error_t & error)
    {
        return get_alias_list(error,
                              affix_binder(qdb_prefix_get, prefix, max_count));
    }

    //! \ingroup client
    //!
    //! \brief Retrieves the count of all entries matching the provided prefix.
    //!
    //! A prefix-based count counts all entries matching a provided prefix.
    //!
    //! \param prefix A pointer to a null-terminated UTF-8 string representing
    //! the desired prefix.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A count of aliases matching the prefix
    qdb_uint_t prefix_count(const char * prefix, qdb_error_t & error)
    {
        qdb_uint_t res = 0;
        error = qdb_prefix_count(_handle, prefix, &res);
        return res;
    }

    //! \ingroup client
    //!
    //! \brief Retrieves the list of all entries matching the provided suffix.
    //!
    //! A suffix-based search will enable you to find all entries matching a
    //! provided \p suffix.
    //!
    //! This function return the list of aliases. It's up to the user to query
    //! the content associated with every entry, if needed.
    //!
    //! \param suffix A pointer to a null-terminated UTF-8 string representing
    //! the desired suffix
    //!
    //! \param max_count An integer limiting the number of results returned by
    //! the function
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return  A vector of string aliases matching the suffix pattern
    std::vector<std::string>
    suffix_get(const char * suffix, qdb_int_t max_count, qdb_error_t & error)
    {
        return get_alias_list(error,
                              affix_binder(qdb_suffix_get, suffix, max_count));
    }

    //! \ingroup client
    //!
    //! \brief Retrieves the count of all entries matching the provided suffix.
    //!
    //! A suffix-based count counts all entries matching a provided suffix.
    //!
    //! \param suffix A pointer to a null-terminated UTF-8 string representing
    //! the desired suffix.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A count of aliases matching the suffix
    qdb_uint_t suffix_count(const char * suffix, qdb_error_t & error)
    {
        qdb_uint_t res = 0;
        error = qdb_suffix_count(_handle, suffix, &res);
        return res;
    }

public:
    //! \ingroup client
    //!
    //! \brief Adds a tag to an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! The tag may or may not exist.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be added.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag to add to the entry
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t attach_tag(const char * alias, const char * tag)
    {
        return qdb_attach_tag(_handle, alias, tag);
    }

    //! \ingroup client
    //!
    //! \brief Tests if an entry has the request tag.
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! If you need to test several entries and/or several tags, consider using
    //! a batch for maximum performance.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be tested.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which presence must be tested
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t has_tag(const char * alias, const char * tag)
    {
        return qdb_has_tag(_handle, alias, tag);
    }

    //! \ingroup client
    //!
    //! \brief Removes a tag from an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! The tag must exist.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be remove.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag to remove.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t detach_tag(const char * alias, const char * tag)
    {
        return qdb_detach_tag(_handle, alias, tag);
    }

private:
    struct get_tagged_binder
    {
        get_tagged_binder(const char * tag) : _tag(tag) {}

        qdb_error_t operator()(qdb_handle_t handle,
                               const char *** results,
                               size_t * result_count)
        {
            return qdb_get_tagged(handle, _tag, results, result_count);
        }

    private:
        const char * _tag;
    };

public:
    //! \ingroup client
    //!
    //! \brief Retrieves all entries that have the specified tag
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The tag must exist.
    //!
    //! The complexity of this function is constant.
    //!
    //! Consider using qdb_tag_iterator_begin if you expect the number of
    //! entries to be very large.
    //!
    //! Consider using iteration on tags for large tags.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A std::vector containing the aliases tagged with the tag.
    std::vector<std::string> get_tagged(const char * tag, qdb_error_t & error)
    {
        return get_alias_list(error, get_tagged_binder(tag));
    }

    //! \ingroup client
    //!
    //! \brief Computes the count of all entries matching the
    //! specified tag, up to the configured max cardinality.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return Number of entries with the input tag
    qdb_uint_t get_tagged_count(const char * tag, qdb_error_t & error)
    {
        qdb_uint_t res = 0;
        error = qdb_get_tagged_count(_handle, tag, &res);
        return res;
    }

    //! \ingroup client
    //!
    //! \brief Computes the approximate count of all entries matching the
    //! specified tag, up to the configured max cardinality.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return Number of entries with the input tag
    qdb_uint_t get_tagged_approximate_count(const char * tag,
                                            qdb_error_t & error)
    {
        qdb_uint_t res = 0;
        error = qdb_get_tagged_approximate_count(_handle, tag, &res);
        return res;
    }

private:
    struct get_tags_binder
    {
        get_tags_binder(const char * alias) : _alias(alias) {}

        qdb_error_t operator()(qdb_handle_t handle,
                               const char *** results,
                               size_t * result_count)
        {
            return qdb_get_tags(handle, _alias, results, result_count);
        }

    private:
        const char * _alias;
    };

public:
    //! \ingroup client
    //!
    //! \brief Retrieves all the tags of an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be added.
    //!
    //! \param error A reference to qdb_error_t error.
    //!
    //! \return A std::vector containing the tags assigned to the alias.
    std::vector<std::string> get_tags(const char * alias, qdb_error_t & error)
    {
        return get_alias_list(error, get_tags_binder(alias));
    }

    //! \ingroup client
    //!
    //! \brief Creates an iterator that will point to the first entry having the
    //!        the specified tag.
    //!
    //! The order in which iteration occurs is unspecified, but entries matching
    //! a tag will never appear twice.
    //!
    //! Only forward iteration is currently supported.
    //!
    //! Once iteration terminates, the caller is responsible for releasing
    //! API-allocated resources with qdb_tag_iterator_close
    //!
    //! If you expect the number of entries to be very small, you might consider
    //! using qdb_get_tagged.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag on which iteration is requested
    //!
    //! \return A constant tag begin iterator
    const_tag_iterator tag_begin(const char * tag)
    {
        return const_tag_iterator(this->_handle, tag);
    }

    //! \ingroup client
    //!
    //! \brief Creates an iterator that will point to the next to last entry
    //! having the specified tag.
    //!
    //! \return A constant tag end iterator
    const_tag_iterator tag_end()
    {
        return const_tag_iterator(this->_handle,
                                  detail::const_tag_iterator_impl::init_end());
    }

public:
    //! \ingroup client
    //!
    //! \brief Removes all the entries on all the nodes of the quasardb cluster.
    //! The function return when the command has been dispatched and executed
    //! on the whole cluster or an error occurred.
    //!
    //! This call is * not* atomic; if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a qdb_e_ok code.
    //!
    //! The handle must be initialized and connected(see connect).
    //!
    //! \return A qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is meant for very specific use cases and its
    //! usage is discouraged.
    qdb_error_t purge_all(int timeout_ms)
    {
        return qdb_purge_all(_handle, timeout_ms);
    }

    //! \ingroup client
    //!
    //! \brief Manually runs the garbage collector, removing stale versions of
    //! entries from the cluster. This may free a small portion of disk space or
    //! memory.
    //!
    //! This call is **not** atomic: if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a qdb_e_ok code.
    //!
    //! The handle must be initialized(see qdb_open and
    //! qdb_open_tcp) and the connection established(see
    //! qdb_connect).
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t trim_all(int timeout_ms)
    {
        return qdb_trim_all(_handle, timeout_ms);
    }

    //! \ingroup client
    //!
    //! \brief Removes all cached data from all the nodes of the cluster.
    //!
    //! This function is disabled on a transient cluster.
    //! Prefer purge_all in this case.
    //!
    //! This call is not atomic: if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a qdb_e_ok code.
    //!
    //! \param timeout_ms A timeout value, in milliseconds.
    //!
    //! \return A qdb_error_t code indicating success or failure.
    qdb_error_t purge_cache(int timeout_ms)
    {
        return qdb_purge_cache(_handle, timeout_ms);
    }

private:
    template <typename T>
    struct cstring_translator
    {
        T operator()(const char * cstr) const
        {
            return T(cstr);
        }
    };

    template <typename Function>
    std::vector<std::string> get_alias_list(qdb_error_t & error, Function f)
    {
        const char ** results = NULL;
        size_t result_count = 0;
        error = f(_handle, &results, &result_count);

        std::vector<std::string> final_result;

        if (error == qdb_e_ok)
        {
            try
            {
                final_result.resize(result_count);
                std::transform(results,
                               results + result_count,
                               final_result.begin(),
                               cstring_translator<std::string>());
            }
            catch (const std::bad_alloc &)
            {
                error = qdb_e_no_memory_local;
                final_result.clear();
            }

            qdb_release(_handle, results);
        }

        return final_result;
    }

private:
    qdb_handle_t _handle;
    int _timeout;
    qdb_encryption_t _encrypt;
    std::string _cluster_pk;
    std::string _user_id;
    std::string _user_sk;
};

typedef qdb::shared_ptr<handle>::type handle_ptr;

} // namespace qdb
