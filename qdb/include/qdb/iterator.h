#ifndef QDB_API_ITERATOR_H
#define QDB_API_ITERATOR_H

//! \file iterator.h
//! \defgroup iterator Iteration-related operations

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

/* explicit packing will prevent some incompatibility cases */
#pragma pack(push)
#pragma pack(8)

    //! \ingroup iterator
    //! \struct qdb_const_iterator_t
    //! \brief A read-only iterator on quasardb entries.
    typedef struct
    {
        //! The handle created with qdb_open
        qdb_handle_t handle;
        //! Opaque token, don't use
        const void * token;

        //! Opaque pointer to internal structure, don't use
        const void * node;
        //! Opaque pointer to reference, don't use
        const void * ref;

        //! The current alias of the entry
        const char * alias;
        //! The content of the current entry
        const char * content;
        //! The size of the content of the current entry
        qdb_size_t content_size;
    } qdb_const_iterator_t;

#pragma pack(pop)

    //! \ingroup iterator
    //! \brief Creates an iterator that points to the "first" entry of the
    //! cluster.
    //!
    //! This function enables you to iterate on all the entries of a cluster.
    //! Keep in mind that there is no guarantee regarding the order of entries.
    //!
    //! The iteration occurs at a fixed point in time, operations occurring
    //! after the iterator has been created will not be visible.
    //!
    //! Call \ref qdb_iterator_next to go to the next available entry, if any.
    //!
    //! When iteration completes the function will return \ref
    //! qdb_e_iterator_end and the user must call \ref qdb_iterator_close to
    //! release client-allocated resources for the iteration.
    //!
    //! Iteration does not allocate any resource on the server.
    //!
    //! Whole cluster iteration is intended for some very specific use case. It
    //! is greatly advised to use tags to work on subgroup of entries.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param iterator A pointer to an uninitialized iterator structure that
    //! will be set to the first entry if successful
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_iterator_next, \ref qdb_iterator_previous, \ref
    //! qdb_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_begin(qdb_handle_t handle, qdb_const_iterator_t * iterator);

    //! \ingroup iterator
    //! \brief Creates an iterator that points to the "last" entry of the
    //! cluster.
    //!
    //! This function enables you to iterate on all the entries of a cluster.
    //! Keep in mind that there is no guarantee regarding the order of entries.
    //!
    //! This works exactly like \ref qdb_iterator_begin, excepts it starts at
    //! the "end" of the cluster.
    //!
    //! Call \ref qdb_iterator_previous to go to the previous available entry,
    //! if any.
    //!
    //! See \ref qdb_iterator_begin for more information.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param iterator A pointer to an uninitialized iterator structure that
    //! will be set to the last entry if successful
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_iterator_begin, \ref qdb_iterator_next, \ref
    //! qdb_iterator_previous, \ref qdb_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_rbegin(qdb_handle_t handle, qdb_const_iterator_t * iterator);

    //! \ingroup iterator
    //! \brief Advances an iterator to the next entry, if any.
    //!
    //! Updates the iterator to point to the next available entry in the
    //! cluster. Although each entry is returned only once, the order in which
    //! entries are returned is undefined. If there is no following entry or it
    //! is otherwise unavailable, the function will return \ref
    //! qdb_e_iterator_end.
    //!
    //! \param iterator A pointer to a previously initialized iterator.
    //!
    //! \see \ref qdb_iterator_begin
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_next(qdb_const_iterator_t * iterator);

    //! \ingroup iterator
    //! \brief Advances an iterator to the previous entry, if any.
    //!
    //! Updates the iterator to point to the previous available entry in the
    //! cluster.
    //! Although each entry is returned only once, the order in which entries
    //! are returned is undefined. If there is no previous entry or it is
    //! otherwise unavailable, the function will return \ref qdb_e_iterator_end.
    //!
    //! \param iterator A pointer to a previously initialized iterator.
    //!
    //! \see \ref qdb_iterator_begin
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_previous(qdb_const_iterator_t * iterator);

    //! \ingroup iterator
    //! \brief Closes a previously initialized iterator and releases all
    //! associated resources.
    //!
    //! \param iterator A pointer to a previously initialized iterator.
    //!
    //! \see \ref qdb_iterator_rbegin, \ref qdb_iterator_begin
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_close(qdb_const_iterator_t * iterator);

    //! \ingroup iterator
    //! \brief Clones a previously initialized iterator.
    //!
    //! Copies the state of the original iterator to a new iterator. Both
    //! iterators can afterward be independently operated. The cloned iterator
    //! will have to be closed with \ref qdb_iterator_close.
    //!
    //! \see \ref qdb_iterator_rbegin, \ref qdb_iterator_begin, \ref
    //! qdb_iterator_close
    QDB_API_LINKAGE qdb_error_t
    qdb_iterator_copy(const qdb_const_iterator_t * original,
                      qdb_const_iterator_t * copy);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_ITERATOR_H */
