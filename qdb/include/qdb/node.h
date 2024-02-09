#ifndef QDB_API_NODE_H
#define QDB_API_NODE_H

//! \file node.h
//! \defgroup node Node- and cluster-related operations

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

    //! \ingroup node
    //! \brief Returns the status of a node.
    //!
    //! The API will request the node for its status and allocate a buffer of
    //! the appropriate size, which will later have to be released using \ref
    //! qdb_release.
    //!
    //! The status is JSON object and contains current information of the node
    //! state, as described in the documentation.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of node for which the status is requested.
    //!
    //! \param[out] content A pointer to the pointer of an API-allocated buffer
    //! holding the node status.
    //!
    //! \param[out] content_length A pointer to an integer representing the size
    //! of the API-allocated buffer.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_node_status(qdb_handle_t handle,
                                                const char * uri,
                                                const char ** content,
                                                qdb_size_t * content_length);

    //! \ingroup node
    //! \brief Returns the configuration of a node.
    //!
    //! The API will request the node for its configuration and allocate a
    //! buffer of the appropriate size, which will later have to be released
    //! using \ref qdb_release.
    //!
    //! The configuration is JSON object, as described in the documentation.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of the node for which the configuration is requested.
    //!
    //! \param[out] content A pointer to the pointer of an API-allocated buffer
    //! holding the node configuration.
    //!
    //! \param[out] content_length A pointer to an integer representing the size
    //! of the API-allocated buffer.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_node_config(qdb_handle_t handle,
                                                const char * uri,
                                                const char ** content,
                                                qdb_size_t * content_length);

    //! \ingroup node
    //! \brief Returns the topology of a node.
    //!
    //! The API will request the node for its topology and allocate a buffer of
    //! the appropriate size, which will later have to be released using \ref
    //! qdb_release.
    //!
    //! The topology is JSON object containing the node address, and the
    //! addresses of its successor and predecessor.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of node for which the topology is requested.
    //!
    //! \param[out] content A pointer to the pointer of an API-allocated buffer
    //! holding the node topology.
    //!
    //! \param[out] content_length A pointer to an integer representing the size
    //! of the API-allocated buffer.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_node_topology(qdb_handle_t handle,
                                                  const char * uri,
                                                  const char ** content,
                                                  qdb_size_t * content_length);

    //! \ingroup node
    //! \brief Requests the node to gracefully stop.
    //!
    //! Use this function to remotely stop a node, the supplied reason will be
    //! displayed in the logs of the remote node.
    //!
    //! By default cluster do not allow this operation and the function will
    //! return \ref qdb_e_operation_disabled error.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of node to be stopped.
    //!
    //! \param reason A pointer to a null-terminated UTF-8 string of the reason
    //! for the stop.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_node_stop(qdb_handle_t handle,
                                              const char * uri,
                                              const char * reason);

    // \ingroup node
    // \brief Interrupts the given partition number on the node.
    //!
    //! Use this function to interrupt a running partition. A negative value
    //! will interrupt all partitions on the node.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A pointer to a null-terminated UTF-8 string representing the
    //! URI of node to be stopped.
    //!
    //! \param partition_number The partition number to interrupt. A negative
    //! value interrupts all partitions.
    //!
    //! \param period The maximum duration into which the interrupt will be
    //! active, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_node_interrupt_partition(qdb_handle_t handle,
                                 const char * uri,
                                 qdb_int16_t partition_number,
                                 qdb_uint_t period);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_NODE_H */
