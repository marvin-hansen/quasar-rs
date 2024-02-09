#ifndef QDB_API_CLIENT_H
#define QDB_API_CLIENT_H

//! \file client.h
//! \defgroup client General client functions

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

#include "error.h"
#include <stddef.h> // NOLINT(modernize-deprecated-headers)
#include <time.h>   // NOLINT(modernize-deprecated-headers)

/*!

\mainpage quasardb C API documentation
\author quasardb SAS
\copyright quasardb SAS All rights reserved

This is a comprehensive reference documentation of the C API. The whole quasardb
documentation is available online at https://doc.quasardb.net/

Function categories:
- \ref client
- \ref double
- \ref error
- \ref attr
- \ref batch
- \ref blob
- \ref integer
- \ref iterator
- \ref log
- \ref node
- \ref option
- \ref perf
- \ref prefix
- \ref query
- \ref suffix
- \ref tag
- \ref timestamp
- \ref ts
*/

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) \
    || defined(__ARM_ARCH_ISA_A64) || defined(__ppc64__)
#    define QDB_PLATFORM_32 0
#    define QDB_PLATFORM_64 1
#else
#    define QDB_PLATFORM_32 1
#    define QDB_PLATFORM_64 0
#endif

//! \ingroup client
//! \typedef qdb_time_t
//! \brief A cross-platform type that represents a time value.
//! \see \ref qdb_expires_at
//! \remark \ref qdb_time_t MUST be 64-bit large. The API will probably not link
//! otherwise.
#ifdef _MSC_VER
    typedef __time64_t qdb_time_t; // NOLINT(modernize-use-using)
#else

#    if QDB_PLATFORM_32
typedef /*unsigned*/ long long qdb_time_t; // NOLINT(modernize-use-using)
#    else
#        include <stdint.h> // NOLINT(modernize-deprecated-headers)
typedef time_t qdb_time_t; // NOLINT(modernize-use-using)
#    endif

#endif

    //! \ingroup client
    //! \typedef qdb_size_t
    //! \brief An alias for size_t
    typedef size_t qdb_size_t; // NOLINT(modernize-use-using)

//! \ingroup client
//! \typedef qdb_int_t
//! \brief A cross-platform type that represents a signed 64-bit integer.
#ifdef _MSC_VER
    typedef __int16 qdb_int16_t;           // NOLINT(modernize-use-using)
    typedef unsigned __int16 qdb_uint16_t; // NOLINT(modernize-use-using)
    typedef __int64 qdb_int_t;             // NOLINT(modernize-use-using)
    typedef unsigned __int64 qdb_uint_t;   // NOLINT(modernize-use-using)
#else
#    include <stdint.h> // NOLINT(modernize-deprecated-headers)
typedef int16_t qdb_int16_t;   // NOLINT(modernize-use-using)
typedef uint16_t qdb_uint16_t; // NOLINT(modernize-use-using)
typedef int64_t qdb_int_t;     // NOLINT(modernize-use-using)
typedef uint64_t qdb_uint_t;   // NOLINT(modernize-use-using)
#endif

//! \ingroup client
//! \def qdb_never_expires
//! \brief An arbitrary time value representing the "no expiration" time value
//! \see \ref qdb_time_t
#define qdb_never_expires ((qdb_time_t)0u)
//! \ingroup client
//! \def qdb_preserve_expiration
//! \brief An arbitrary time value representing the "preserve existing
//! expiration" time value
//! \see \ref qdb_time_t
#define qdb_preserve_expiration ((qdb_time_t)0xFFFFFFFFFFFFFFFFULL)

#define qdb_min_time ((qdb_time_t)0x8000000000000000LL)
#define qdb_max_time ((qdb_time_t)0xFFFFFFFFFFFFFFFFLL)

    //! \ingroup client
    //! \brief An enumeration of API limits
    typedef enum qdb_limits_t // NOLINT(modernize-use-using)
    {
        //! The maximum allowed length for aliases.
        qdb_l_max_alias_length = 1024,
        //! \deprecated Please use \ref qdb_l_max_alias_length.
        qdb_l_alias_max_length = qdb_l_max_alias_length,
        //! The maximum allowed length of a user name.
        qdb_l_max_user_name_length = 120,
        //! \deprecated Please use \ref qdb_l_max_user_name_length.
        qdb_max_user_name_length = qdb_l_max_user_name_length
    } qdb_limits_t;

    //! \ingroup client
    //! \typedef qdb_protocol_t
    //! \brief An enumeration of allowed network protocols.
    //! \see \ref qdb_open
    typedef enum qdb_protocol_t // NOLINT(modernize-use-using)
    {
        //! Uses TCP/IP to communicate with the cluster.
        //! This is currently the only supported network protocol.
        qdb_p_tcp = 0
    } qdb_protocol_t;

    //! \ingroup client
    //! \typedef qdb_handle_t
    //! \brief An opaque handle to internal API-allocated structures needed for
    //! maintaining connection to a cluster.
    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct qdb_handle_internal * qdb_handle_t;

/* explicit packing will prevent some incompatibility cases */
#pragma pack(push)
#pragma pack(8)

    //! \ingroup client
    //! \struct qdb_timespec_t
    //! \brief A structure representing an elapsed time since epoch
    //! (cross-platform equivalent of timespec structure)
    typedef struct // NOLINT(modernize-use-using)
    {
        //! number of whole seconds of elapsed time
        qdb_time_t tv_sec;
        //! rest of the elapsed time, represented as a number of nanoseconds,
        //! always less than one billion
        qdb_time_t tv_nsec;
    } qdb_timespec_t;

#define qdb_min_timespec           \
    qdb_timespec_t                 \
    {                              \
        qdb_min_time, qdb_min_time \
    }

#define qdb_max_timespec           \
    qdb_timespec_t                 \
    {                              \
        qdb_max_time, qdb_max_time \
    }

    //! \ingroup client
    //! \struct qdb_remote_node_t
    //! \brief A structure representing the address of a quasardb node.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! A pointer to a null-terminated string representing the
        //! address of the remote node
        const char * address;
#if QDB_PLATFORM_32
        char _padding0[8 - sizeof(const char *)];
#endif
        //! The port of the remote node
        unsigned short port;
#if QDB_PLATFORM_32
        char _padding1[8 - sizeof(const char *)];
#endif
    } qdb_remote_node_t;

    //! \ingroup client
    //! \typedef qdb_entry_type_t
    //! \brief A enumeration representing possible entries type.
    typedef enum qdb_entry_type_t // NOLINT(modernize-use-using)
    {
        //! Uninitialized value.
        qdb_entry_uninitialized = -1,
        //! Binary large object (blob).
        qdb_entry_blob = 0,
        //! Signed 64-bit integer.
        qdb_entry_integer = 1,
        //! Distributed hash set.
        qdb_entry_hset = 2,
        //! Tag.
        qdb_entry_tag = 3,
        //! Distributed double-entry queue (deque).
        qdb_entry_deque = 4,
        //! Distributed binary stream.
        qdb_entry_stream = 5,
        //! Distributed time series.
        qdb_entry_ts = 6,
        //! Double-precision floating point number.
        qdb_entry_double = 10,
        //! 128-bit timestamp with nanosecond precision.
        qdb_entry_timestamp = 11,
        //! UTF-8 string.
        qdb_entry_string = 12,

        qdb_entry_internal_ts_double_bucket = 20,
        qdb_entry_internal_ts_blob_bucket = 21,
        qdb_entry_internal_ts_int64_bucket = 22,
        qdb_entry_internal_ts_timestamp_bucket = 23,
        qdb_entry_internal_ts_string_bucket = 24,
        qdb_entry_internal_ts_double_bucket_index = 30,
        qdb_entry_internal_ts_blob_bucket_index = 31,
        qdb_entry_internal_ts_int64_bucket_index = 32,
        qdb_entry_internal_ts_timestamp_bucket_index = 33,
        qdb_entry_internal_ts_string_bucket_index = 34,
        qdb_entry_internal_directory = 50,
    } qdb_entry_type_t;

    //! \ingroup client
    //! \struct qdb_id_t
    //! \brief A cluster-wide unique identifier.
    typedef struct // NOLINT(modernize-use-using)
    {
        qdb_int_t data[4];
    } qdb_id_t;

    //! \ingroup client
    //! \brief A structure representing a character string of the given length.
    typedef struct // NOLINT(modernize-use-using)
    {
        const char * data;
        qdb_size_t length;
    } qdb_string_t;

#if QDB_PLATFORM_32
#    define QDB_STRING_OFFSETOF_DATA 0
#    define QDB_STRING_OFFSETOF_LENGTH 4
#else
#    define QDB_STRING_OFFSETOF_DATA 0
#    define QDB_STRING_OFFSETOF_LENGTH 8
#endif

    //! \ingroup client
    //! \struct qdb_entry_metadata_t
    //! \brief A structure representing the metadata of an entry in the
    //! database.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! Entry key (alias).
        qdb_string_t alias;
#if QDB_PLATFORM_32
        // Add padding to make alias + padding have the same size on 32-bit and
        // 64-bit platforms.
        char _padding0[16 - sizeof(qdb_string_t)];
#endif

        //! unique identifier
        qdb_id_t reference;

        //! The type of the entry
        qdb_entry_type_t type;
#if QDB_PLATFORM_32
        // Padding (alignas(8)) required to make sure the structure is 8-bytes
        // aligned since qdb_entry_type_t is 4-byte when compiling for 32-bit
        // platforms.
        char _padding1[8 - sizeof(qdb_entry_type_t)];
#endif

        //! content size for blobs and integers, N/A for other types
        qdb_uint_t size;

        //! last time the entry was modified
        //! May not reflect the actual modification time for distributed
        //! containers such as deque and hset.
        qdb_timespec_t modification_time;
        //! The expiry time of the entry, if any.
        qdb_timespec_t expiry_time;
    } qdb_entry_metadata_t;

#pragma pack(pop)

    //! \ingroup client
    //! \brief Returns a null-terminated string describing the API version
    //!
    //! \return A static, null-terminated string describing the API version.
    //! The buffer is API managed and should not be freed or written to by the
    //! caller.
    QDB_API_LINKAGE const char * qdb_version(void);

    //! \ingroup client
    //! \brief Returns a null-terminated string describing the exact API build
    //!
    //! \return A static, null-terminated string describing the exact API build.
    //! The buffer is API managed and should not be freed or written to by the
    //! caller.
    QDB_API_LINKAGE const char * qdb_build(void);

    //! \ingroup client
    //! \brief Creates a \ref qdb_handle_t. No connection will be established.
    //!
    //! \param[out] handle A pointer to a \ref qdb_handle_t that will be ready
    //! to connect to a cluster.
    //!
    //! \param proto The protocol to use.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_connect, \ref qdb_open_tcp, \ref qdb_protocol_t
    QDB_API_LINKAGE qdb_error_t qdb_open(qdb_handle_t * handle,
                                         qdb_protocol_t proto);

    //! \ingroup client
    //! \brief Creates a TCP/IP qdb_handle_t. No connection will be established.
    //!
    //! \return An initialized qdb_handle_t, ready to connect, in case of
    //! success, NULL in case of failure.
    //!
    //! \see \ref qdb_connect, \ref qdb_open
    QDB_API_LINKAGE qdb_handle_t qdb_open_tcp(void);

    //! \ingroup client
    //! \brief Describes the last error code returned from the given handle.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param[out] error An optional pointer to a \ref qdb_error_t that will be
    //! set to the error code of the last API call with the given handle.
    //!
    //! \param[out] message An optional pointer to a pointer that will be set to
    //! a \ref qdb_string_t which stores the message describing the last API
    //! call with the given handle. The client is required to release the memory
    //! owned by this pointer once it's no longer used. Memory will not be
    //! allocated for an empty internal error message.
    //!
    //! \see \ref qdb_release
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    QDB_API_LINKAGE qdb_error_t qdb_get_last_error(qdb_handle_t handle,
                                                   qdb_error_t * error,
                                                   qdb_string_t ** message);

    //! \ingroup client
    //! \brief Binds the client instance to a quasardb cluster and connect to at
    //! least one node within.
    //!
    //! Quasardb URI are in the form `qdb://<address>:<port>` where `<address>`
    //! is either an IPv4 or IPv6 (surrounded with square brackets), or a domain
    //! name. It is recommended to specify multiple addresses should the
    //! designated node be unavailable.
    //!
    //! URI examples:
    //!   - `qdb://myserver.org:2836` - Connects to myserver.org on the port
    //!   2836
    //!   - `qdb://127.0.0.1:2836` - Connects to the local IPv4 loopback on the
    //!   port 2836
    //!   - `qdb://myserver1.org:2836,myserver2.org:2836` - Connects to
    //!   myserver1.org
    //!   or myserver2.org on the port 2836
    //!   - `qdb://[::1]:2836` - Connects to the local IPv6 loopback on the port
    //!   2836
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of the quasardb cluster to connect to.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_open, \ref qdb_open_tcp
    //! \attention Make sure all the addresses in the URI belong to the same
    //! cluster
    QDB_API_LINKAGE qdb_error_t qdb_connect(qdb_handle_t handle,
                                            const char * uri);

    //! \ingroup client
    //! \brief Closes the handle previously opened with \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! This results in terminating all connections and releasing all internal
    //! buffers, including buffers which may
    //! have been allocated as or a result of batch operations or get
    //! operations.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention API-allocated buffers may be released by this call. For
    //! example, the buffer allocated by \ref qdb_blob_get may be released by
    //! this call.
    QDB_API_LINKAGE qdb_error_t qdb_close(qdb_handle_t handle);

    //! \ingroup client
    //! \brief Creates a clone of a buffer using API's high-performance memory
    //! allocator.
    //!
    //! The allocated buffer has to be released later with \ref qdb_release.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param source_buffer A pointer to a buffer to clone
    //!
    //! \param source_buffer_size The size of the buffer to clone
    //!
    //! \param[out] dest_buffer A pointer to a a pointer of an API-allocated
    //! buffer whose content will be a copy of the source buffer
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_copy_alloc_buffer(qdb_handle_t handle,
                          const void * source_buffer,
                          qdb_size_t source_buffer_size,
                          void ** dest_buffer);

    //! \ingroup client
    //! \brief Allocates a buffer managed by the Quasar API.
    //!
    //! The allocated buffer has to be released later with \ref qdb_release.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param buffer_size The size of the buffer to allocate, greater
    //! than 0.
    //!
    //! \param[out] dest_buffer A pointer to a a pointer of an API-allocated
    //! buffer of the given size. The content of the buffer is undefined.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_alloc_buffer(qdb_handle_t handle,
                                                 qdb_size_t buffer_size,
                                                 void ** dest_buffer);

    //! \ingroup client
    //! \brief Releases an API-allocated buffer.
    //!
    //! Failure to properly call this function may result in excessive memory
    //! usage. Most operations that return a content (e.g. batch operations,
    //! \ref qdb_blob_get, \ref qdb_blob_get_and_update, \ref
    //! qdb_blob_compare_and_swap...) will allocate a buffer for the content and
    //! will not release the allocated buffer until you either call this
    //! function or close the handle.
    //!
    //! The function will be able to release any kind of buffer allocated by a
    //! quasardb API call, whether it's a single buffer, an array or an array of
    //! buffers.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param buffer A pointer to an API-allocated buffer to release. The
    //! behavior for non API buffers is undefined.
    QDB_API_LINKAGE void qdb_release(qdb_handle_t handle, const void * buffer);

    //! \ingroup client
    //! \brief Removes an entry from the cluster, regardless of its type.
    //!
    //! This call will remove the entry, whether it is a blob, integer, deque,
    //! or hset. It will properly untag the entry.
    //! If the entry spawns on multiple entries or nodes (deques and hsets),
    //! all blocks will be properly removed.
    //!
    //! The call is ACID, regardless of the type of the entry and a transaction
    //! will be created if need be.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_remove(qdb_handle_t handle,
                                           const char * alias);

    //! \ingroup client
    //! \brief Sets the absolute expiration time of an entry, if the type
    //! supports expiration.
    //!
    //! Blobs and integers can have an expiration time and will be automatically
    //! removed by the cluster when they expire.
    //!
    //! The absolute expiration time is the Unix epoch, that is, the number of
    //! milliseconds since 1 January 1970, 00:00:00 UTC.
    //! To use a relative expiration time (that is expiration relative to the
    //! time of the call), use \ref qdb_expires_from_now.
    //!
    //! To remove the expiration time of an entry, specify the value
    //! \ref qdb_never_expires as \p expiry_time parameter.
    //!
    //! Values in the past are refused, but the cluster will have a certain
    //! tolerance to account for clock skews.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param expiry_time The new, absolute UTC expiration time.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention It is an error to specify an expiration in the past, although
    //! the cluster has a certain tolerance to account for clock synchronization
    //! \see \ref qdb_expires_from_now
    QDB_API_LINKAGE qdb_error_t qdb_expires_at(qdb_handle_t handle,
                                               const char * alias,
                                               qdb_time_t expiry_time);

    //! \ingroup client
    //! \brief Sets the expiration time of an entry, relative to the current
    //! time of the client, if the type supports expiration.
    //!
    //! Blobs and integers can have an expiration time and will automatically be
    //! removed by the cluster when they expire.
    //!
    //! The expiration is relative to the current time of the machine.
    //!
    //! To remove the expiration time of an entry or to use an absolute
    //! expiration time use \ref qdb_expires_at.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param expiry_delta The number of milliseconds, relative to the current
    //! time, after which the entry should expire.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_expires_at
    QDB_API_LINKAGE qdb_error_t qdb_expires_from_now(qdb_handle_t handle,
                                                     const char * alias,
                                                     qdb_time_t expiry_delta);

    //! \ingroup client
    //! \brief Retrieves the absolute expiration time of the given entry.
    //!
    //! The returned expiration time is the Unix epoch, UTC.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param[out] expiry_time A pointer to an expiry time which will be set to
    //! the expiration of the entry if the call is successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_expires_from_now
    //! \see \ref qdb_expires_at
    //! \see \ref qdb_get_metadata
    QDB_DEPRECATED("use qdb_get_metadata")
    QDB_API_LINKAGE qdb_error_t qdb_get_expiry_time(qdb_handle_t handle,
                                                    const char * alias,
                                                    qdb_time_t * expiry_time);

    //! \ingroup client
    //! \brief Returns the primary node of an entry.
    //!
    //! The exact location of an entry should be assumed random and users should
    //! not bother about its location as the API will transparently locate the
    //! best node for the requested operation.
    //!
    //! This function is intended for higher level APIs that need to optimize
    //! transfers and potentially push computation close to the data.
    //!
    //! This function allocates memory for the null terminated address string
    //! call qdb_release on the location structure to release memory.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param[out] location A pointer to a \ref qdb_remote_node_t structure
    //! that will receive the address of the primary node of the entry if
    //! successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_get_location(qdb_handle_t handle,
                                                 const char * alias,
                                                 qdb_remote_node_t * location);

    //! \ingroup client
    //! \brief Gets the type of an entry, if it exists.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param[out] entry_type A pointer to a \ref qdb_entry_type_t that will
    //! receive the type of the entry if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_metadata
    QDB_DEPRECATED("use qdb_get_metadata")
    QDB_API_LINKAGE qdb_error_t qdb_get_type(qdb_handle_t handle,
                                             const char * alias,
                                             qdb_entry_type_t * entry_type);

    //! \ingroup client
    //! \brief Gets the meta-information about an entry, if it exists.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string
    //! representing the alias of the entry.
    //!
    //! \param[out] entry_metadata A pointer to a \ref qdb_entry_metadata_t that
    //! will receive the metadata of the entry if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_get_metadata(qdb_handle_t handle,
                     const char * alias,
                     qdb_entry_metadata_t * entry_metadata);

    //! \ingroup client
    //! \brief Gets the meta-information about an entry, if it exists.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param reference A pointer to a structure representing the internal
    //! reference of the entry.
    //!
    //! \param[out] entry_metadata A pointer to a \ref qdb_entry_metadata_t that
    //! will receive the metadata of the entry if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_get_metadata_by_id(qdb_handle_t handle,
                           const qdb_id_t * reference,
                           qdb_entry_metadata_t * entry_metadata);

    //! \ingroup client
    //! \brief Removes irremediably all data from all the nodes of the cluster.
    //!
    //! This function is useful when quasardb is used as a cache and is not the
    //! golden source.
    //!
    //! This call is not atomic: if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a \ref qdb_e_ok code.
    //!
    //! By default cluster does not allow this operation and the function
    //! returns a \ref qdb_e_operation_disabled error.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param timeout_ms A timeout value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention Use this function at your own risk. This function causes
    //! irremediable data loss.
    QDB_API_LINKAGE qdb_error_t qdb_purge_all(qdb_handle_t handle,
                                              int timeout_ms);

    //! \ingroup client
    //! \brief Trims all data on all the nodes of the cluster.
    //!
    //! quasardb uses Multi-Version Concurrency Control (MVCC) as a foundation
    //! of its transaction engine. It will automatically clean up old versions
    //! as entries are accessed.
    //!
    //! This call is not atomic: if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a \ref qdb_e_ok code.
    //!
    //! Entries that are not accessed may not be cleaned up, resulting in
    //! increasing disk usage.
    //!
    //! This function will request each node to trim all entries and release
    //! unused memory. Because this operation is I/O and CPU intensive, it is
    //! not recommended to run it when the cluster is heavily used.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param pause_ms A pause interval, in milliseconds, between trimming
    //! individual entries.
    //!
    //! \param timeout_ms A timeout value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention This function impacts the performance of the cluster.
    //! \see \ref qdb_cluster_compact
    QDB_API_LINKAGE qdb_error_t qdb_trim_all(qdb_handle_t handle,
                                             int pause_ms,
                                             int timeout_ms);

    //! \ingroup client
    //! \brief Get the trimming process progress.
    //!
    //! This function requests each nodes to check the trimming process
    //! status. \see \ref qdb_cluster_compact
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param run The trimming process status.
    //! If 0 than the trimming process is finished else it is running.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_trim_all_progress(qdb_handle_t handle,
                                                      qdb_uint_t * run);

    //! \ingroup client
    //! \brief Abort the running trimming process.
    //!
    //! This function requests each nodes to abort the trimming process.
    //! This function aborts the last active trimming process.
    //! \see \ref qdb_cluster_compact
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_abort_trim_all(qdb_handle_t handle);

    //! \ingroup client
    //! \brief Trims specific key
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param bytes_trimmed A pointer to unsigned int to store number of
    //! bytes trimmed
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_trim_entry(qdb_handle_t handle,
                                               const char * alias,
                                               qdb_uint_t * bytes_trimmed);

    //! \ingroup client
    //! \typedef qdb_compact_options_t
    //! \brief An enumeration of compact options
    //! \see \ref qdb_cluster_compact
    typedef enum qdb_compact_options_t // NOLINT(modernize-use-using)
    {
        //! Compact the entire database in a single operation
        qdb_compact_full = 0,
        //! Compact the database one slice at a time (reduces peak disk usage
        //! during compaction)
        qdb_compact_piecewise = 1,
        //! Compact the only specified column family
        qdb_compact_cf = 2,
        //! Compact the database for the only specified key prefix
        qdb_compact_prefix = 3,
    } qdb_compact_options_t;

    //! \ingroup client
    //! \brief The optional parameters for a compact operation
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The compaction mode.
        qdb_compact_options_t options;

        union
        {
            //! Parameter for the compaction mode \ref qdb_compact_cf.
            //! A pointer to a null-terminated UTF-8 string representing
            //! the family name.
            const char * column_family;

            //! Parameter for the compaction mode \ref qdb_compact_prefix.
            //! A pointer to a null-terminated UTF-8 string representing
            //! the desired key prefix.
            const char * key_prefix;
        } params;
    } qdb_compact_params_t;

    //! \ingroup client
    //! \brief Compacts all data in the persistence layer on all the nodes of
    //! the cluster.
    //!
    //! This function will request each nodes to compact files on disk. Because
    //! this operation is I/O and CPU intensive.
    //!
    //! Triggers a compaction. The function returns right after the compaction
    //! is started and will not wait for its completion. Only one compaction
    //! per cluster can be running at any point in time. Returns an error if
    //! a compaction is already in progress.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param params The compaction parameters (optional).
    //! If not defined then the mode is \ref qdb_compact_full
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention This function impacts the performance of the cluster.
    //! \see \ref qdb_trim_all
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_compact(qdb_handle_t handle,
                        const qdb_compact_params_t * params);

    //! \ingroup client
    //! \brief Get the compaction process progress.
    //!
    //! This function requests each node to check the compaction process
    //! status. \see \ref qdb_cluster_compact
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param run The compaction process status.
    //! If 0 than the compaction process is finished else it is ran.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_get_compact_progress(qdb_handle_t handle, qdb_uint_t * run);

    //! \ingroup client
    //! \brief Abort the running compaction process.
    //!
    //! This function requests each node to abort the compaction process.
    //! This function aborts the last active compact process.
    //! \see \ref qdb_cluster_compact
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_cluster_abort_compact(qdb_handle_t handle);

    //! \ingroup client
    //! \brief For the read only cluster call the synchronization with the
    //! master DB.
    //!
    //! This function will request each nodes to synchronize with master DB.
    //! Because this operation is I/O and CPU intensive.
    //!
    //! Triggers a synchronization. The function returns right after the
    //! synchronization is started and will not wait for its completion. Only
    //! one synchronization per cluster can be running at any point in time.
    //! Returns an error if a synchronization is already in progress.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \attention This function impacts the performance of the cluster.
    //! \see \ref qdb_trim_all
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_sync_with_master(qdb_handle_t handle);

    //! \ingroup client
    //! \brief For the read only cluster get the synchronization with the master
    //! DB process progress.
    //!
    //! This function requests each node to check the synchronization process
    //! status. \see \ref qdb_cluster_sync_with_master
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param run The synchronization process status.
    //! If 0 than the synchronization process is finished else it is ran.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_get_sync_with_master_progress(qdb_handle_t handle,
                                              qdb_uint_t * run);

    //! \ingroup client
    //! \brief Removes all cached data from all the nodes of the cluster.
    //!
    //! This function is disabled on a transient cluster.
    //! Prefer purge_all in this case.
    //!
    //! This call is not atomic: if the command cannot be dispatched on the
    //! whole cluster, it will be dispatched on as many nodes as possible and
    //! the function will return with a \ref qdb_e_ok code.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param timeout_ms A timeout value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_purge_cache(qdb_handle_t handle,
                                                int timeout_ms);

    //! \ingroup client
    //! \brief Wait for all nodes of the cluster to be stabilized.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param timeout_ms A timeout value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_wait_for_stabilization(qdb_handle_t handle,
                                                           int timeout_ms);

    //! \ingroup client
    //! \brief Retrieve a sorted array of endpoints.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param[out] endpoints A pointer to a \ref qdb_remote_node_t array
    //! that will receive the addresses of the nodes of the cluster
    //!
    //! \param[out] endpoints_count A pointer that will be filled with number of
    //! nodes in the array
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_endpoints(qdb_handle_t handle,
                          qdb_remote_node_t ** endpoints,
                          qdb_size_t * endpoints_count);

    //! \ingroup client
    //! \brief Reload user configuration from file.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_cluster_reload_user_config(qdb_handle_t handle);

#ifdef __cplusplus
} /* extern "C" */
#endif

#include "option.h"

#endif /* QDB_API_CLIENT_H */
