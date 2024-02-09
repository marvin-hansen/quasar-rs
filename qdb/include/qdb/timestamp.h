#ifndef QDB_API_TIMESTAMP_H
#define QDB_API_TIMESTAMP_H

//! \file timestamp.h
//! \defgroup timestamp Double-precision floating point numbers

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

#include "client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //! \ingroup timestamp
    //! \brief Creates a new 128-bit timestamp.
    //!
    //! Atomically creates an entry of the given alias and sets it to a
    //! cross-platform 128-bit timestamp.
    //! If the entry already exists, the function returns an error.
    //!
    //! You can specify an expiry or use qdb_never_expires if you don't want the
    //! entry to expire.
    //!
    //! If you want to create or update an entry use \ref qdb_timestamp_update.
    //!
    //! The value will be correctly translated independently of the endianness
    //! of the client's platform.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param value The value to which the number must be set.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_timestamp_update, \ref qdb_timestamp_get, \ref
    //! qdb_timestamp_add
    QDB_API_LINKAGE qdb_error_t qdb_timestamp_put(qdb_handle_t handle,
                                                  const char * alias,
                                                  const qdb_timespec_t * value,
                                                  qdb_time_t expiry_time);

    //! \ingroup timestamp
    //! \brief Creates or updates a 128-bit timestamp.
    //!
    //! Atomically updates an entry of the given alias to the provided value.
    //! If the entry doesn't exist, it will be created.
    //! If the entry is not a timestamp, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! You can specify an expiry time or use qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param value The value to which the timestamp must be set.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_timestamp_put, \ref qdb_timestamp_get, \ref
    //! qdb_timestamp_add
    QDB_API_LINKAGE qdb_error_t
    qdb_timestamp_update(qdb_handle_t handle,
                         const char * alias,
                         const qdb_timespec_t * value,
                         qdb_time_t expiry_time);

    //! \ingroup timestamp
    //! \brief Retrieves the value of a 128-bit timestamp.
    //!
    //! Atomically retrieves the value of an existing 128-bit timestamp.
    //!
    //! If the entry is not a timestamp, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param value A pointer to a timestamp that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_timestamp_put, \ref qdb_timestamp_update, \ref
    //! qdb_timestamp_add
    QDB_API_LINKAGE qdb_error_t qdb_timestamp_get(qdb_handle_t handle,
                                                  const char * alias,
                                                  qdb_timespec_t * value);

    //! \ingroup timestamp
    //! \brief Atomically increases or decreases a 128-bit timestamp.
    //!
    //! The specified entry will be atomically increased (or decreased)
    //! according to the given addend value:
    //!  - To increase the value, specify a positive addend
    //!  - To decrease the value, specify a negative addend
    //!
    //! The function return the result of the operation.
    //!
    //! The entry must already exist.
    //! If the entry is not a timestamp, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param addend The value to add to the existing value.
    //!
    //! \param result A pointer to a timestamp that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_timestamp_put, \ref qdb_timestamp_update, \ref
    //! qdb_timestamp_get
    QDB_API_LINKAGE qdb_error_t qdb_timestamp_add(qdb_handle_t handle,
                                                  const char * alias,
                                                  const qdb_timespec_t * addend,
                                                  qdb_timespec_t * result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_TIMESTAMP_H */
