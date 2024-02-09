#ifndef QDB_API_SUFFIX_H
#define QDB_API_SUFFIX_H

//! \file suffix.h
//! \defgroup suffix Suffix-based operations

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

    //! \ingroup suffix
    //! \brief Retrieves the list of all entries matching the provided suffix.
    //!
    //! A suffix-based search will enable you to find all entries matching a
    //! provided \p suffix.
    //!
    //! This function returns the list of aliases. It's up to the user to query
    //! the content associated with every entry, if needed.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param suffix A pointer to a null-terminated UTF-8 string representing
    //! the desired suffix
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
    QDB_API_LINKAGE qdb_error_t qdb_suffix_get(qdb_handle_t handle,
                                               const char * suffix,
                                               qdb_int_t max_count,
                                               const char *** results,
                                               size_t * result_count);

    //! \ingroup suffix
    //! \brief Retrieves the count of all entries matching the provided suffix.
    //!
    //! A suffix-based count counts all entries matching a provided suffix.
    //!
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param suffix A pointer to a null-terminated UTF-8 string representing
    //! the desired suffix
    //!
    //! \param[out] result_count A pointer to an integer giving the results
    //! count
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_suffix_count(qdb_handle_t handle,
                                                 const char * suffix,
                                                 qdb_uint_t * result_count);

    //! \ingroup suffix
    //! \brief Retrieves the *approximate* count of all entries matching the
    //! provided suffix.
    //!
    //! A suffix-based count counts all entries matching a provided suffix.
    //!
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param suffix A pointer to a null-terminated UTF-8 string representing
    //! the desired suffix
    //!
    //! \param[out] result_count A pointer to an integer giving the results
    //! count
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_suffix_approximate_count(qdb_handle_t handle,
                                 const char * suffix,
                                 qdb_uint_t * result_count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
