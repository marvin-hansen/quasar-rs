#ifndef QDB_API_DOUBLE_H
#define QDB_API_DOUBLE_H

//! \file double.h
//! \defgroup double Double-precision floating point numbers

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

    //! \ingroup double
    //! \brief Creates a new double-precision floating point number.
    //!
    //! Atomically creates an entry of the given alias and sets it to a
    //! cross-platform double-precision floating point number.
    //! If the entry already exists, the function returns an error.
    //!
    //! You can specify an expiry or use qdb_never_expires if you don't want the
    //! entry to expire.
    //!
    //! If you want to create or update an entry use \ref qdb_double_update.
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
    //! \see \ref qdb_double_update, \ref qdb_double_get, \ref qdb_double_add
    QDB_API_LINKAGE qdb_error_t qdb_double_put(qdb_handle_t handle,
                                               const char * alias,
                                               double value,
                                               qdb_time_t expiry_time);

    //! \ingroup double
    //! \brief Creates or updates a double-precision floating point number.
    //!
    //! Atomically updates an entry of the given alias to the provided value.
    //! If the entry doesn't exist, it will be created.
    //! If the entry is not a double, the function will return
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
    //! \param value The value to which the number must be set.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_double_put, \ref qdb_double_get, \ref qdb_double_add
    QDB_API_LINKAGE qdb_error_t qdb_double_update(qdb_handle_t handle,
                                                  const char * alias,
                                                  double value,
                                                  qdb_time_t expiry_time);

    //! \ingroup double
    //! \brief Retrieves the value of a double-precision floating point number.
    //!
    //! Atomically retrieves the value of an existing double-precision floating
    //! point number.
    //!
    //! If the entry is not a double, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param value A pointer to a number that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_double_put, \ref qdb_double_update, \ref qdb_double_add
    QDB_API_LINKAGE qdb_error_t qdb_double_get(qdb_handle_t handle,
                                               const char * alias,
                                               double * value);

    //! \ingroup double
    //! \brief Atomically increases or decreases a double-precision floating
    //! point number.
    //!
    //! The specified entry will be atomically increased (or decreased)
    //! according to the given addend value:
    //!  - To increase the value, specify a positive addend
    //!  - To decrease the value, specify a negative addend
    //!
    //! The function return the result of the operation.
    //!
    //! The entry must already exist.
    //! If the entry is not a double, the function will return
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
    //! \param result A pointer to a double that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_double_put, \ref qdb_double_update, \ref qdb_double_get
    QDB_API_LINKAGE qdb_error_t qdb_double_add(qdb_handle_t handle,
                                               const char * alias,
                                               double addend,
                                               double * result);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_DOUBLE_H */
