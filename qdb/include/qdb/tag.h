#ifndef QDB_API_TAG_H
#define QDB_API_TAG_H

//! \file tag.h
//! \defgroup tag Tagging operations

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

    //! \ingroup tag
    //! \brief Adds a tag to an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! The tag may or may not exist.
    //!
    //! Consider using \ref qdb_attach_tags if you are adding several tags to
    //! the same entry.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be added.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag to add to the entry
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_attach_tags, \ref qdb_has_tag, \ref qdb_detach_tag, \ref
    //! qdb_get_tagged, \ref qdb_get_tags
    QDB_API_LINKAGE qdb_error_t qdb_attach_tag(qdb_handle_t handle,
                                               const char * alias,
                                               const char * tag);

    //! \ingroup tag
    //! \brief Adds a collection of tags to a single entry.
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The function will ignore existing tags.
    //!
    //! The entry must exist.
    //!
    //! The tag may or may not exist.
    //!
    //! Consider using \ref qdb_attach_tag if you are adding a single tag.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tags must be added.
    //!
    //! \param tags A pointer to an array of null-terminated UTF-8 strings
    //! representing the tags to add to the entry
    //!
    //! \param tag_count The count of tags to add to the entry.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_attach_tag, \ref qdb_has_tag, \ref qdb_detach_tags, \ref
    //! qdb_get_tagged, \ref qdb_get_tags
    QDB_API_LINKAGE qdb_error_t qdb_attach_tags(qdb_handle_t handle,
                                                const char * alias,
                                                const char * const * tags,
                                                size_t tag_count);

    //! \ingroup tag
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
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be tested.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which presence must be tested
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tagged, \ref qdb_get_tags, \ref qdb_run_batch
    QDB_API_LINKAGE qdb_error_t qdb_has_tag(qdb_handle_t handle,
                                            const char * alias,
                                            const char * tag);

    //! \ingroup tag
    //! \brief Removes a tag from an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! The tag must exist.
    //!
    //! Consider using \ref qdb_detach_tags if you are removing several tags at
    //! once.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be remove.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag to remove.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_detach_tags, \ref qdb_get_tagged, \ref qdb_get_tags
    QDB_API_LINKAGE qdb_error_t qdb_detach_tag(qdb_handle_t handle,
                                               const char * alias,
                                               const char * tag);

    //! \ingroup tag
    //! \brief Removes a collection of tags from a single entry.
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! The tags must exist.
    //!
    //! Consider using \ref qdb_detach_tag if you are removing a single tag.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tags must be removed.
    //!
    //! \param tags A pointer to an array of null-terminated UTF-8 strings
    //! representing the tags to remove.
    //!
    //! \param tag_count The count of tags to remove to the entry.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_detach_tag, \ref qdb_get_tagged, \ref qdb_get_tags
    QDB_API_LINKAGE qdb_error_t qdb_detach_tags(qdb_handle_t handle,
                                                const char * alias,
                                                const char * const * tags,
                                                size_t tag_count);

    //! \ingroup tag
    //! \brief Retrieves all entries that have the specified tag
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The tag must exist.
    //!
    //! The complexity of this function is constant.
    //!
    //! Consider using \ref qdb_tag_iterator_begin if you expect the number of
    //! entries to be very large.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param[out] aliases A pointer to an array of null-terminated UTF-8
    //! string that will list the aliases of the entries having the requested
    //! tag.
    //!
    //! \param[out] alias_count A pointer to an integer that will receive the
    //! number of returned aliases.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tags, \ref qdb_tag_iterator_begin, \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_get_tagged(qdb_handle_t handle,
                                               const char * tag,
                                               const char *** aliases,
                                               size_t * alias_count);

    //! \ingroup tag
    //! \brief Computes the count of all entries matching the
    //! specified tag, up to the configured max cardinality.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param[out] count A pointer to an integer that will receive the count of
    //! tags
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tags, \ref qdb_get_tagged, \ref
    //! qdb_option_set_max_cardinality
    QDB_API_LINKAGE qdb_error_t qdb_get_tagged_count(qdb_handle_t handle,
                                                     const char * tag,
                                                     qdb_uint_t * count);

    //! \ingroup tag
    //! \brief Computes the approximate count of all entries matching the
    //! specified tag, up to the configured max cardinality.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag for which to search entries.
    //!
    //! \param[out] count A pointer to an integer that will receive the count of
    //! tags
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tags, \ref qdb_get_tagged, \ref
    //! qdb_option_set_max_cardinality
    QDB_API_LINKAGE qdb_error_t
    qdb_get_tagged_approximate_count(qdb_handle_t handle,
                                     const char * tag,
                                     qdb_uint_t * count);

    //! \ingroup tag
    //! \brief Retrieves all the tags of an entry
    //!
    //! Tagging an entry enables you to search for entries based on their tags.
    //! Tags scale across nodes.
    //!
    //! The entry must exist.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry to which the tag must be added.
    //!
    //! \param[out] tags A pointer to an array of null-terminated UTF-8 string
    //! that will list the tags of the entry
    //!
    //! \param[out] tag_count A pointer to an integer that will receive the
    //! number of returned tags.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tagged, \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_get_tags(qdb_handle_t handle,
                                             const char * alias,
                                             const char *** tags,
                                             size_t * tag_count);

#pragma pack(push)
#pragma pack(8)

    //! \ingroup tag
    //! \brief An iterator to iterate on the entries matching a certain tag.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! the associated handle
        //! set by \ref qdb_tag_iterator_begin
        qdb_handle_t handle;
        //! reserved do not use
        //! set by \ref qdb_tag_iterator_begin
        const void * token;
        //! reserved do not use
        //! set by \ref qdb_tag_iterator_begin
        qdb_size_t magic;

        //! The alias of the current entry
        const char * alias;
        //! The type of the current entry
        qdb_entry_type_t type;
    } qdb_const_tag_iterator_t;

#pragma pack(pop)

    //! \ingroup tag
    //! \brief Creates an iterator that will point to the first entry having the
    //!        the specified tag.
    //!
    //! The order in which iteration occurs is unspecified, but entries matching
    //! a tag will never appear twice.
    //!
    //! Only forward iteration is currently supported.
    //!
    //! Once iteration terminates, the caller is responsible for releasing
    //! API-allocated resources with \ref qdb_tag_iterator_close
    //!
    //! If you expect the number of entries to be very small, you might consider
    //! using \ref qdb_get_tagged.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param tag A pointer to a null-terminated UTF-8 string representing the
    //! tag on which iteration is requested
    //!
    //! \param[out] iterator A pointer to an uninitialized \ref
    //! qdb_const_tag_iterator_t
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tagged, \ref qdb_tag_iterator_next, \ref
    //! qdb_tag_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_tag_iterator_begin(qdb_handle_t handle,
                           const char * tag,
                           qdb_const_tag_iterator_t * iterator);

    //! \ingroup tag
    //! \brief Advance a previously initialized iterator to the next entry.
    //!
    //! The order in which iteration occurs is unspecified, but entries matching
    //! a tag will never appear twice.
    //!
    //! Only forward iteration is currently supported.
    //!
    //! Once iteration terminates, the caller is responsible for releasing
    //! API-allocated resources with \ref qdb_tag_iterator_close
    //!
    //! If you expect the number of entries to be very small, you might consider
    //! using \ref qdb_get_tagged.
    //!
    //! \param[in,out] iterator A pointer to an initialized \ref
    //! qdb_const_tag_iterator_t
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_get_tagged, \ref qdb_tag_iterator_begin, \ref
    //! qdb_tag_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_tag_iterator_next(qdb_const_tag_iterator_t * iterator);

    //! \ingroup tag
    //! \brief Releases all API-allocated resources during a previous iteration.
    //!
    //! Once iteration terminates, the caller is responsible for releasing
    //! API-allocated resources with \ref qdb_tag_iterator_close
    //!
    //! \param[in,out] iterator A pointer to an initialized \ref
    //! qdb_const_tag_iterator_t
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_tag_iterator_begin
    QDB_API_LINKAGE qdb_error_t
    qdb_tag_iterator_close(qdb_const_tag_iterator_t * iterator);

    //! \ingroup client
    //! \brief Clones a previously initialized tag iterator.
    //!
    //! Copies the state of the original iterator to a new iterator. Both
    //! iterators can be independently operated afterwards. The cloned iterator
    //! will have to be closed with \ref qdb_tag_iterator_close.
    //!
    //! \param original The original iterator to copy, source.
    //!
    //! \param[out] copy An uninitialized iterator to be set, destination.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_tag_iterator_begin, \ref qdb_tag_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_tag_iterator_copy(const qdb_const_tag_iterator_t * original,
                          qdb_const_tag_iterator_t * copy);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
