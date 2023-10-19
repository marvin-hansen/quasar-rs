#ifndef QDB_API_DIRECT_H
#define QDB_API_DIRECT_H

//! \file direct.h
//! \defgroup direct Direct node storage access

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

#include "client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //! \ingroup client
    //! \typedef qdb_direct_handle_t
    //! \brief An opaque handle to internal API-allocated structures needed for
    //! maintaining a direct connection to a cluster.
    typedef struct qdb_direct_handle_internal * qdb_direct_handle_t;

    //! \ingroup direct
    //! \brief Opens a direct connection to a node for use with the direct API.
    //!
    //! The returned handled must be freed with qdb_direct_close. Releasing the
    //! direct handle has no impact on non-direct connections or other direct
    //! handles.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A string representing the node address and port (without the
    //! qdb:// prefix)
    //!
    //! \return A valid qdb_direct_handle_t if successful, a null pointer.
    QDB_API_LINKAGE qdb_direct_handle_t qdb_direct_connect(qdb_handle_t handle,
                                                           const char * uri);

    //! \ingroup direct
    //! \brief Close a direct handle initially opened with qdb_direct_connect.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    QDB_API_LINKAGE void qdb_direct_close(qdb_direct_handle_t handle);

    //! \ingroup direct
    //! \brief Retrieves an entry's content from a quasardb node's local
    //! storage.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content. This buffer must be released by the caller with a call to
    //! \ref qdb_release().
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string
    //! representing the alias of the entry.
    //!
    //! \param[out] content A pointer to a pointer that will be set to a
    //! function-allocated buffer holding the entry's content.
    //!
    //! \param[out] content_length A pointer to a \ref qdb_size_t that will be
    //! set to the content's size, in bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_blob_get(qdb_direct_handle_t handle,
                        const char * alias,
                        const void ** content,
                        qdb_size_t * content_length);

    //! \ingroup direct
    //! \brief Creates a new entry and sets its content to the provided blob
    //!
    //! If the entry already exists the function will fail and will return
    //! \ref qdb_e_alias_already_exists.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of blobs.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the blob content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_direct_blob_put(qdb_direct_handle_t handle,
                                                    const char * alias,
                                                    const void * content,
                                                    qdb_size_t content_length,
                                                    qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Creates or updates an entry and sets its content to the provided
    //! blob
    //!
    //! If the entry already exists, the function will modify the entry.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of blobs.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the blob content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_blob_update(qdb_direct_handle_t handle,
                           const char * alias,
                           const void * content,
                           qdb_size_t content_length,
                           qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Removes an entry from the local node's storage, regardless of its
    //! type.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The call is ACID, regardless of the type of the entry and a transaction
    //! will be created if need be.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_direct_remove(qdb_direct_handle_t handle,
                                                  const char * alias);

    //! \ingroup direct
    //! \brief Creates a new signed 64-bit integer.
    //!
    //! Atomically creates an entry of the given alias and sets it to a
    //! cross-platform signed 64-bit integer.
    //! If the entry already exists, the function returns an error.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use qdb_never_expires if you don't want the
    //! entry to expire.
    //!
    //! If you want to create or update an entry use \ref qdb_int_update.
    //!
    //! The value will be correctly translated independently of the endianness
    //! of the client's platform.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer The value to which the integer must be set.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_update, \ref qdb_int_get, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_put(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t integer,
                                                   qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Creates or updates a signed 64-bit integer.
    //!
    //! Atomically updates an entry of the given alias to the provided value.
    //! If the entry doesn't exist, it will be created.
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry time or use qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer The value to which the integer must be set.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_get, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_int_update(qdb_direct_handle_t handle,
                          const char * alias,
                          qdb_int_t integer,
                          qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Retrieves the value of a signed 64-bit integer.
    //!
    //! Atomically retrieves the value of an existing 64-bit integer.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer A pointer to an integer that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_update, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_get(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t * integer);

    //! \ingroup direct
    //! \brief Atomically increases or decreases a signed 64-bit integer.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The specified entry will be atomically increased (or decreased)
    //! according to the given addend value:
    //!  - To increase the value, specify a positive addend
    //!  - To decrease the value, specify a negative addend
    //!
    //! The function return the result of the operation.
    //!
    //! The entry must already exist.
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param addend The value to add to the existing value.
    //!
    //! \param result A pointer to an integer that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_update, \ref qdb_int_get
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_add(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t addend,
                                                   qdb_int_t * result);

    //! \ingroup prefix
    //! \brief Retrieves the list of all entries matching the provided prefix.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! A prefix-based search will enable you to find all entries matching a
    //! provided \p prefix.
    //!
    //! This function returns the list of aliases. It's up to the user to query
    //! the content associated with every entry, if needed.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param prefix A pointer to a null-terminated UTF-8 string representing
    //! the desired prefix
    //!
    //! \param max_count An integer limiting the number of results returned by
    //! the function
    //!
    //! \param[out] results A pointer to a list of results
    //!
    //! \param[out] result_count A pointer to an integer giving the results
    //! count
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_prefix_get(qdb_direct_handle_t handle,
                          const char * prefix,
                          qdb_int_t max_count,
                          const char *** results,
                          size_t * result_count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
