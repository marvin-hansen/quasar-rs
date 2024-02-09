#ifndef QDB_API_STRING_H
#define QDB_API_STRING_H

//! \file string.h
//! \defgroup string String operations

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

    //! \ingroup string
    //! \brief Retrieves an entry's content from the quasardb server. The caller
    //!        is responsible for allocating and freeing the provided buffer.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! If the buffer is not large enough to hold the data, the function will
    //! fail and return \ref qdb_e_buffer_too_small. \p content_length will
    //! nevertheless be updated with entry size so that the caller may resize
    //! its buffer and try again.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string
    //! representing the alias of the entry.
    //!
    //! \param[in,out] content A pointer to a user-allocated buffer that will
    //! receive the entry's content.
    //!
    //! \param[out] content_length A pointer to a \ref qdb_size_t initialized
    //! with the length of the destination buffer, in bytes. It will be updated
    //! with the length of the retrieved content, even if the buffer is not
    //! large enough to hold all the data.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_string_get_noalloc(qdb_handle_t handle,
                           const char * alias,
                           char * content,
                           qdb_size_t * content_length);

    //! \ingroup string
    //! \brief Retrieves an entry's content from the quasardb server.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content. This buffer must be released by the caller with a call to
    //! \ref qdb_release().
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
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
    QDB_API_LINKAGE qdb_error_t qdb_string_get(qdb_handle_t handle,
                                               const char * alias,
                                               const char ** content,
                                               qdb_size_t * content_length);

    //! \ingroup string
    //! \brief Retrieves a part of an entry's content from the quasardb server.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content. This buffer must be released by the caller with a call to
    //! \ref qdb_release().
    //!
    //! The returned content will be a subset of the entry's data in the range
    //! <tt>[</tt>\p offset<tt>, </tt>\p offset<tt> + *</tt>\p
    //! content_length<tt>)</tt>.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param[out] content A pointer to a pointer that will be set to a
    //! function-allocated buffer holding the entry's content.
    //!
    //! \param[out] content_length A pointer to a \ref qdb_size_t that holds the
    //! length of the requested content and that will be set to the returned
    //! content's size, in bytes. If content length is equal to zero or is
    //! greater than entry's length minus offset, then all the content from
    //! offset until the end will be returned.
    //!
    //! \param offset Offset from which to get content, in bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_reserved_string_get(qdb_handle_t handle,
                            const char * alias,
                            const char ** content,
                            qdb_size_t * content_length,
                            qdb_size_t offset);

    //! \ingroup string
    //! \brief Atomically gets an entry from the quasardb server and removes it.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content. This buffer must be released by the caller with a call to
    //! \ref qdb_release().
    //!
    //! If the buffer is not large enough to hold the data, the function will
    //! fail and return \ref qdb_e_buffer_too_small. \p content_length will
    //! nevertheless be updated with entry size so that the caller may resize
    //! its buffer and try again.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
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
    qdb_string_get_and_remove(qdb_handle_t handle,
                              const char * alias,
                              const char ** content,
                              qdb_size_t * content_length);

    //! \ingroup string
    //! \brief Creates a new entry and sets its content to the provided string
    //!
    //! If the entry already exists the function will fail and will return
    //! \ref qdb_e_alias_already_exists.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of strings.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the string content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_string_put(qdb_handle_t handle,
                                               const char * alias,
                                               const char * content,
                                               qdb_size_t content_length,
                                               qdb_time_t expiry_time);

    //! \ingroup string
    //! \brief Creates or updates an entry and sets its content to the provided
    //! string
    //!
    //! If the entry already exists, the function will modify the entry.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of strings.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the string content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_string_update(qdb_handle_t handle,
                                                  const char * alias,
                                                  const char * content,
                                                  qdb_size_t content_length,
                                                  qdb_time_t expiry_time);

    //! \ingroup string
    //! \brief This function is reserved for internal use and its usage is
    //! discouraged.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the content to merge
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param offset Offset at which merge content
    //!
    //! \param truncate 1 - the entry will be truncated at the end of the new
    //! buffer.
    //!                 0 - the original content will be preserved.
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_reserved_string_merge(qdb_handle_t handle,
                              const char * alias,
                              const char * content,
                              qdb_size_t content_length,
                              qdb_size_t offset,
                              int truncate,
                              qdb_time_t expiry_time);

    //! \ingroup string
    //! \brief Atomically gets and updates (in this order) the entry on the
    //! quasardb server.
    //!
    //! The entry must already exist.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param update_content A pointer to the buffer with the new string
    //! content
    //!
    //! \param update_content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \param[out] get_content A pointer to a pointer to an API-allocated
    //! buffer holding the entry content, before the update.
    //!
    //! \param[out] get_content_length A pointer to a \ref qdb_size_t that will
    //! be set to the content's size, in bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_string_get_and_update(qdb_handle_t handle,
                              const char * alias,
                              const char * update_content,
                              qdb_size_t update_content_length,
                              qdb_time_t expiry_time,
                              const char ** get_content,
                              qdb_size_t * get_content_length);

    //! \ingroup string
    //! \brief Atomically compares the entry with comparand and updates it to
    //!        \p new_value if, and only if, they match.
    //!
    //! The function returns the original value of the entry in case of a
    //! mismatch. When it matches, no content is returned.
    //!
    //! The entry must already exist.
    //!
    //! Update will occur if and only if the content of the entry matches bit
    //! for bit the content of the comparand buffer.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param new_value A pointer to a buffer that represents the entry's
    //! content to be updated to the server in case of match.
    //!
    //! \param new_value_length The length of the buffer, in bytes
    //!
    //! \param comparand A pointer to a buffer that represents the entry's
    //! content to be compared to.
    //!
    //! \param comparand_length The length of the buffer, in bytes
    //!
    //! \param expiry_time An optional absolute expiry time expressed in UTC
    //! UNIX epoch.
    //!
    //! \param[out] original_value A pointer to a pointer that will be set to a
    //! function-allocated buffer holding the entry's original content, before
    //! the update, if any.
    //!
    //! \param[out] original_value_length A pointer to a \ref qdb_size_t that
    //! will be set to the content's size, in bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_string_compare_and_swap(qdb_handle_t handle,
                                const char * alias,
                                const char * new_value,
                                qdb_size_t new_value_length,
                                const char * comparand,
                                qdb_size_t comparand_length,
                                qdb_time_t expiry_time,
                                const char ** original_value,
                                qdb_size_t * original_value_length);

    // partial comparand, complete swap
    QDB_API_LINKAGE qdb_error_t
    qdb_reserved_string_cas_partial(qdb_handle_t handle,
                                    const char * alias,
                                    const char * new_value,
                                    qdb_size_t new_value_length,
                                    const char * comparand,
                                    qdb_size_t comparand_length,
                                    qdb_size_t comparand_offset,
                                    qdb_time_t expiry_time,
                                    const char ** original_value,
                                    qdb_size_t * original_value_length);

    //! \ingroup string
    //! \brief Atomically removes the entry on the server if the content
    //! matches.
    //!
    //! The entry must already exist.
    //!
    //! Removal will occur if and only if the content of the entry matches bit
    //! for bit the content of the comparand buffer.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string
    //! representing the alias of the entry.
    //!
    //! \param comparand A pointer to a buffer to use a comparand
    //!
    //! \param comparand_length The length of the buffer, in bytes
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_string_remove_if(qdb_handle_t handle,
                         const char * alias,
                         const char * comparand,
                         qdb_size_t comparand_length);

    //! \ingroup string
    //! \brief Retrieves all strings that have content matching the specified
    //! pattern
    //!
    //! \warning This function is experimental.
    //!
    //! The complexity of this function is linear in the number of entries.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param pattern A pointer to the pattern buffer to search for
    //!
    //! \param pattern_length The length of the pattern buffer, in bytes
    //!
    //! \param max_count An integer limiting the number of results returned by
    //! the function
    //!
    //! \param[out] aliases A pointer to an array of null-terminated UTF-8
    //! string that will list the aliases of the entries matching the requested
    //! pattern.
    //!
    //! \param[out] alias_count A pointer to an integer that will receive the
    //! number of returned aliases.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_string_scan(qdb_handle_t handle,
                                                const char * pattern,
                                                qdb_size_t pattern_length,
                                                qdb_int_t max_count,
                                                const char *** aliases,
                                                size_t * alias_count);

    //! \ingroup string
    //! \brief   Retrieves all strings that have content matching the specified
    //! regular expression (regex)
    //!
    //! \warning This function is experimental.
    //!
    //! The complexity of this function is linear in the number of entries.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param pattern A pointer to a null-terminated UTF-8 string representing
    //! the regular expression to search for. The regex syntax is
    //! defined by ECMA-262 grammar.
    //!
    //! \param max_count An integer limiting the number of results returned by
    //! the function.
    //!
    //! \param[out] aliases A pointer to an array of null-terminated UTF-8
    //! string that will list the aliases of the entries matching the requested
    //! pattern.
    //!
    //! \param[out] alias_count A pointer to an integer that will receive the
    //! number of returned aliases.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see http://ecma-international.org/ecma-262/5.1/#sec-15.10
    QDB_API_LINKAGE qdb_error_t qdb_string_scan_regex(qdb_handle_t handle,
                                                      const char * pattern,
                                                      qdb_int_t max_count,
                                                      const char *** aliases,
                                                      size_t * alias_count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
