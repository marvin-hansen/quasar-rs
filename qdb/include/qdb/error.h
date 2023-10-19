#ifndef QDB_API_ERROR_H
#define QDB_API_ERROR_H

//! \file error.h
//! \defgroup error Error codes and error handling

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

#include "_attr.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //  Error code are 32 bit integers computed as follow
    //
    //  High order to low order
    //
    //  4-bit - origin
    //
    //      1111 - Remote system
    //      1110 - Local system
    //      1101 - Connection
    //      1100 - Input
    //      1011 - Entry
    //
    //  4-bit - severity
    //
    //      0011 - Unrecoverable
    //      0010 - Error
    //      0001 - Warning - Nothing bad but you might not have the result you
    //      want 0000 - Info
    //
    //  8-bit - Reserved
    //  16-bit - Code - The actual error code
    //
    // You are highly ENCOURAGED to use macros to check for errors as in the
    // future error handling may change.

    // clang-format off

//! \ingroup error
//! \brief The error origin.
typedef enum qdb_error_origin_t
{
    qdb_e_origin_system_remote      = (int) 0xF0000000,
    qdb_e_origin_system_local       = (int) 0xE0000000,
    qdb_e_origin_connection         = (int) 0xD0000000,
    qdb_e_origin_input              = (int) 0xC0000000,
    qdb_e_origin_operation          = (int) 0xB0000000,
    qdb_e_origin_protocol           = (int) 0xA0000000
} qdb_error_origin_t;

//! \ingroup error
//! \def QDB_ERROR_ORIGIN
//! \brief Extracts the origin out of a \ref qdb_error_t
#define QDB_ERROR_ORIGIN(x)         ((x) & (int)0xF0000000)

//! \ingroup error
//! \brief An error severity level.
typedef enum qdb_error_severity_t
{
    qdb_e_severity_unrecoverable    = 0x03000000,
    qdb_e_severity_error            = 0x02000000,
    qdb_e_severity_warning          = 0x01000000,
    qdb_e_severity_info             = 0x00000000
} qdb_error_severity_t;

//! \ingroup error
//! \def QDB_ERROR_SEVERITY
//! \brief Extracts the severity out of a \ref qdb_error_t
#define QDB_ERROR_SEVERITY(x)       ((x) & (int)0x0F000000)

#define _QDB_EC(origin, severity, code) (((int)origin) | ((int)severity) | code)

//! \ingroup error
//! \brief An error code indicating success or failure.
typedef enum qdb_error_t
{
    //! Success.
    qdb_e_ok = 0,

    // ------------------------------------------------------------------------------------------------------
    //  error name                      = origin                     | severity                     | code
    // ------------------------------------------------------------------------------------------------------
    //! Uninitialized error.
    qdb_e_uninitialized                 = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_unrecoverable, 0xFFFF),
    //! Entry alias/key was not found.
    qdb_e_alias_not_found               = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0008),
    //! Entry alias/key already exists.
    qdb_e_alias_already_exists          = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0009),
    //! Index out of bounds.
    qdb_e_out_of_bounds                 = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_warning      , 0x0019),
    //! Skipped operation. Used in batches and transactions.
    qdb_e_skipped                       = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0021),
    //! Entry or column is incompatible with the operation.
    qdb_e_incompatible_type             = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0022),
    //! Container is empty.
    qdb_e_container_empty               = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0023),
    //! Container is full.
    qdb_e_container_full                = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0024),
    //! Element was not found.
    qdb_e_element_not_found             = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x0025),
    //! Element already exists.
    qdb_e_element_already_exists        = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x0026),
    //! Arithmetic operation overflows.
    qdb_e_overflow                      = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0027),
    //! Arithmetic operation underflows.
    qdb_e_underflow                     = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0028),
    //! Tag is already set.
    qdb_e_tag_already_set               = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x0029),
    //! Tag is not set.
    qdb_e_tag_not_set                   = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x002A),
    //! Operation timed out.
    qdb_e_timeout                       = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x000A),
    //! Connection was refused.
    qdb_e_connection_refused            = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_unrecoverable, 0x000E),
    //! Connection was reset.
    qdb_e_connection_reset              = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x000F),
    //! Cluster is unstable.
    qdb_e_unstable_cluster              = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x0012),
    //! Please retry.
    qdb_e_try_again                     = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x0017),
    //! There is another ongoing conflicting operation.
    qdb_e_conflict                      = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x001A),
    //! Handle is not connected.
    qdb_e_not_connected                 = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x001B),
    //! Resource is locked.
    qdb_e_resource_locked               = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x002D),
    //! System error on remote node (server-side).
    //! Please check `errno` or `GetLastError()` for actual error.
    qdb_e_system_remote                 = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0001),
    //! System error on local system (client-side).
    //! Please check `errno` or `GetLastError()` for actual error.
    qdb_e_system_local                  = _QDB_EC(qdb_e_origin_system_local , qdb_e_severity_unrecoverable, 0x0001),
    //! Internal error on remote node (server-side).
    qdb_e_internal_remote               = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0002),
    //! Internal error on local system (client-side).
    qdb_e_internal_local                = _QDB_EC(qdb_e_origin_system_local , qdb_e_severity_unrecoverable, 0x0002),
    //! No memory on remote node (server-side).
    qdb_e_no_memory_remote              = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0003),
    //! No memory on local system (client-side).
    qdb_e_no_memory_local               = _QDB_EC(qdb_e_origin_system_local , qdb_e_severity_unrecoverable, 0x0003),
    //! Protocol is invalid.
    qdb_e_invalid_protocol              = _QDB_EC(qdb_e_origin_protocol     , qdb_e_severity_unrecoverable, 0x0004),
    //! Host was not found.
    qdb_e_host_not_found                = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x0005),
    //! Buffer is too small.
    qdb_e_buffer_too_small              = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_warning      , 0x000B),
    //! Operation is not implemented.
    qdb_e_not_implemented               = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0011),
    //! Version is invalid.
    qdb_e_invalid_version               = _QDB_EC(qdb_e_origin_protocol     , qdb_e_severity_unrecoverable, 0x0016),
    //! Argument is invalid.
    qdb_e_invalid_argument              = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0018),
    //! Handle is invalid.
    qdb_e_invalid_handle                = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x001C),
    //! Alias/key is reserved.
    qdb_e_reserved_alias                = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x001D),
    //! Content did not match.
    qdb_e_unmatched_content             = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x001E),
    //! Iterator is invalid.
    qdb_e_invalid_iterator              = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x001F),
    //! Entry is too large.
    qdb_e_entry_too_large               = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x002B),
    //! Transaction failed partially.
    //! \warning This may provoke failures until the transaction has not been rolled back.
    //! \see Cluster configuration parameter `global/cluster/max_transaction_duration`.
    qdb_e_transaction_partial_failure   = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x002C),
    //! Operation has not been enabled in cluster configuration.
    qdb_e_operation_disabled            = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x002E),
    //! Operation is not permitted.
    qdb_e_operation_not_permitted       = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x002F),
    //! Iterator reached the end.
    qdb_e_iterator_end                  = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x0030),
    //! Cluster sent an invalid reply.
    qdb_e_invalid_reply                 = _QDB_EC(qdb_e_origin_protocol     , qdb_e_severity_unrecoverable, 0x0031),
    //! Success. A new entry has been created.
    qdb_e_ok_created                    = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_info         , 0x0032),
    //! No more space on disk.
    qdb_e_no_space_left                 = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0033),
    //! Disk space quota has been reached.
    qdb_e_quota_exceeded                = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0034),
    //! Alias is too long.
    //! \see \ref qdb_l_max_alias_length
    qdb_e_alias_too_long                = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0035),
    //! Cluster nodes have important clock differences.
    qdb_e_clock_skew                    = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_error        , 0x0036),
    //! Access is denied.
    qdb_e_access_denied                 = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x0037),
    //! Login failed.
    qdb_e_login_failed                  = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_error        , 0x0038),
    //! Column was not found.
    qdb_e_column_not_found              = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_warning      , 0x0039),
    //! Query is too complex.
    qdb_e_query_too_complex             = _QDB_EC(qdb_e_origin_operation    , qdb_e_severity_error        , 0x0040),
    //! Security key is invalid.
    qdb_e_invalid_crypto_key            = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0041),
    //! Malformed query
    qdb_e_invalid_query                 = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0042),
    //! Malformed regex
    qdb_e_invalid_regex                 = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0043),
    //! Unknown user
    qdb_e_unknown_user                  = _QDB_EC(qdb_e_origin_input        , qdb_e_severity_error        , 0x0044),
    //! Task has been interrupted
    qdb_e_interrupted                   = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_error        , 0x0045),
    //! Client network buffer is too small
    qdb_e_network_inbuf_too_small       = _QDB_EC(qdb_e_origin_system_local , qdb_e_severity_error        , 0x0046),
    //! Generic network error
    qdb_e_network_error                 = _QDB_EC(qdb_e_origin_connection   , qdb_e_severity_error        , 0x0047),
    //! Corrupted data
    qdb_e_data_corruption               = _QDB_EC(qdb_e_origin_system_remote, qdb_e_severity_unrecoverable, 0x0048)
} qdb_error_t;

#undef _QDB_EC

// clang-format on

//! \ingroup error
//! \def QDB_SUCCESS
//! \brief True if and only if the \ref qdb_error_t indicates a success
#define QDB_SUCCESS(x) \
    ((x == qdb_e_ok) || (QDB_ERROR_SEVERITY(x) == qdb_e_severity_info))

//! \ingroup error
//! \def QDB_FAILURE
//! \brief True if and only if the \ref qdb_error_t indicates a failure
#define QDB_FAILURE(x) (!QDB_SUCCESS(x))

    //! \ingroup error
    //! \brief An alias for \ref qdb_error_t
    typedef qdb_error_t qdb_status_t;

    //! \ingroup client
    //! \brief Translates an error code into an English error message
    //!
    //! \param error The \ref qdb_error_t code outputted by another function
    //!
    //! \return A static, null-terminated string describing the error.
    //! The buffer is API managed and should not be freed or written to by the
    //! caller.
    QDB_API_LINKAGE const char * qdb_error(qdb_error_t error);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // QDB_API_ERROR_H
