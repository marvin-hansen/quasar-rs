#ifndef QDB_API_OPTION_H
#define QDB_API_OPTION_H

//! \file option.h
//! \defgroup option Options

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

    //! \ingroup option
    //! \brief Sets the timeout of all network operations.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param timeout_ms The timeout of network operation, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark The lower the timeout, the higher the risk of having timeout
    //! errors.
    QDB_API_LINKAGE qdb_error_t qdb_option_set_timeout(qdb_handle_t handle,
                                                       int timeout_ms);

    //! \ingroup option
    //! \brief Gets the timeout of all network operations.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param[out] timeout_ms A pointer to the variable that will receive the
    //! timeout value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_option_get_timeout(qdb_handle_t handle,
                                                       int * timeout_ms);

    //! \ingroup option
    //! \brief Sets the maximum incoming buffer size for all network operations
    //! of the client.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param max_size The maximum input size in bytes
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark Only modify this setting if you expect to receive very large
    //! answers from the server.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_client_max_in_buf_size(qdb_handle_t handle, size_t max_size);

    //! \ingroup option
    //! \brief Gets the maximum incoming buffer size for all network operations
    //! of the client.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param max_size A pointer that will receive the current maximum
    //! incoming buffer size.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_client_max_in_buf_size(qdb_handle_t handle,
                                          size_t * max_size);

    //! \ingroup option
    //! \brief Gets the maximum incoming buffer size for all network operations
    //! of the remote cluster.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param max_size A pointer that will receive the current maximum
    //! incoming buffer size.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_cluster_max_in_buf_size(qdb_handle_t handle,
                                           size_t * max_size);

    //! \ingroup option
    //! \brief Sets the maximum stabilization waiting time for operations.
    //!
    //! This value has no impact for operations on a stable cluster. It must
    //! higher than the timeout value.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param wait_ms The maximum amount of time to wait, in ms.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark The lower the value, the higher the chance of having
    //! "cluster unstable" errors
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_stabilization_max_wait(qdb_handle_t handle, int wait_ms);

    //! \ingroup option
    //! \brief Gets the maximum stabilization waiting time for operations.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param[out] wait_ms A pointer to the variable that will receive the
    //! waiting value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_stabilization_max_wait(qdb_handle_t handle, int * wait_ms);

    //! \ingroup option
    //! \brief Sets the maximum transaction retry time for operations.
    //!
    //! This value has no impact when transaction do not conflict.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param wait_ms The maximum amount of time to wait, in ms.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark The lower the value, the higher the chance of having
    //! "conflicting transactions" errors.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_transaction_max_wait(qdb_handle_t handle, int wait_ms);

    //! \ingroup option
    //! \brief Gets the maximum transaction retry time for operations.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp.
    //!
    //! \param[out] wait_ms A pointer to the variable that will receive the
    //! waiting value, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_transaction_max_wait(qdb_handle_t handle, int * wait_ms);

    //! \ingroup option
    //! \brief Sets the maximum allowed cardinality of a quasardb query. The
    //! default value is 100'003. The minimum allowed value is 100.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp
    //!
    //! \param max_cardinality The maximum cardinality of a query.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark Improper usage of this function may cause a denial of service on
    //! the client.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_max_cardinality(qdb_handle_t handle,
                                   qdb_uint_t max_cardinality);

    //! \ingroup option
    //! \brief Sets the number of threads that will be used to execute queries
    //! by the current handle.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp
    //!
    //! \param thread_count Number of threads. Value of 0 means the number of
    //! logical processor cores.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark Improper usage of this function may cause performance issues.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_client_max_parallelism(qdb_handle_t handle,
                                          qdb_size_t thread_count);

    //! \ingroup option
    //! \brief Gets the number of threads that will be used to execute queries
    //! by the current handle.
    //!
    //! \param handle The qdb_handle_t that was initialized with \ref qdb_open
    //! or \ref qdb_open_tcp
    //!
    //! \param[out] thread_count A pointer to variable that will receive the
    //! number of threads.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //!
    //! \remark Improper usage of this function may cause performance issues.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_client_max_parallelism(qdb_handle_t handle,
                                          qdb_size_t * thread_count);

    //! \ingroup option
    //! \brief Gets the state of the load-balancing.
    //!
    //! By default the client will not use automatic load-balancing.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param[out] is_enabled A pointer to variable that will receive an
    //! integer indicating whether the load-balacing is enabled (positive) or
    //! disabled (zero).
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_option_disable_client_load_balancing
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_client_load_balancing(qdb_handle_t handle, int * is_enabled);

    //! \ingroup option
    //! \brief Enables load-balancing for all threads.
    //!
    //! By default the client will not use automatic load-balancing.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_option_disable_client_load_balancing
    QDB_API_LINKAGE qdb_error_t
    qdb_option_enable_client_load_balancing(qdb_handle_t handle);

    //! \ingroup option
    //! \brief Disables load-balancing for all threads.
    //!
    //! By default the client will not use automatic load-balancing.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_option_enable_client_load_balancing
    QDB_API_LINKAGE qdb_error_t
    qdb_option_disable_client_load_balancing(qdb_handle_t handle);

    //! \ingroup option
    //! \brief An enumeration of compression parameters
    //! \see \ref qdb_option_set_compression
    typedef enum qdb_compression_t
    {
        //! No compression
        qdb_comp_none = 0,
        //! Maximum compression speed, potentially minimum compression ratio.
        //! This is currently the default.
        qdb_comp_fast = 1,
        //! Maximum compression ratio, potentially minimum compression speed.
        //! This is currently not implemented.
        qdb_comp_best = 2
    } qdb_compression_t;

    //! \ingroup option
    //! \brief Set the compression level for all future messages emitted by the
    //! specified handle. Regardless of this parameter, the API will
    //! be able to read whatever compression the server uses.
    //!
    //! \param handle The handle on which to set the compression level
    //!
    //! \param comp_level The compression level to use
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_compression_t
    //! \remark For better performance, consider disabling compression if your
    //! data is already compressed.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_compression(qdb_handle_t handle,
                               qdb_compression_t comp_level);

    //! \ingroup option
    //! \brief An enumeration of encryption parameters
    //! \see \ref qdb_option_set_encryption
    typedef enum qdb_encryption_t
    {
        qdb_crypt_none = 0,       //!< No encryption.
        qdb_crypt_aes_gcm_256 = 1 //!< AEC-GCM 256-bit encryption.
    } qdb_encryption_t;

    //! \ingroup option
    //! \brief Set the encryption method for all future messages emitted by the
    //! specified handle.
    //!
    //! \see \ref qdb_encryption_t
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_encryption(qdb_handle_t handle, qdb_encryption_t encryption);

    //! \ingroup option
    //! \brief Set the public key of the cluster used for authentification of
    //! nodes on connection.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_cluster_public_key(qdb_handle_t handle,
                                      const char * public_key);

    //! \ingroup option
    //! \brief Set the user credentials (name and private key) used for
    //! authentification of the user on connection.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_user_credentials(qdb_handle_t handle,
                                    const char * user_name,
                                    const char * private_key);

    //! \ingroup option
    //! \brief Set the user credentials and cluster public key used for
    //! authentification of the user on connection.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_load_security_files(qdb_handle_t handle,
                                   const char * cluster_public_key_file,
                                   const char * user_credentials_file);

    //! \ingroup option
    //! \brief Sets the timeout of \ref qdb_ts_batch_push_async operation.
    //!
    //! \param handle The handle on which to set the batch async push timeout
    //!
    //! \param timeout_ms The timeout to set, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \remark The lower the timeout, the higher the risk of having
    //! timeout errors.
    //! \see qdb_ts_batch_push_async.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_set_ts_batch_push_async_timeout(qdb_handle_t handle,
                                               int timeout_ms);

    //! \ingroup option
    //! \brief Gets the timeout of \ref qdb_ts_batch_push_async operation.
    //!
    //! \param handle The handle on which to get the batch async push timeout
    //!
    //! \param[out] timeout_ms A pointer to the timeout, in milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see qdb_ts_batch_push_async.
    QDB_API_LINKAGE qdb_error_t
    qdb_option_get_ts_batch_push_async_timeout(qdb_handle_t handle,
                                               int * timeout_ms);

    //! \ingroup option
    //! \brief Enables automatic compactions in the cluster's persistence.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see A \ref qdb_option_cluster_disable_auto_compaction
    QDB_API_LINKAGE qdb_error_t
    qdb_option_cluster_enable_auto_compaction(qdb_handle_t handle);

    //! \ingroup option
    //! \brief Disables automatic compactions in the cluster's persistence.
    //! Manual compaction is still possible.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see A \ref qdb_option_cluster_enable_auto_compaction
    QDB_API_LINKAGE qdb_error_t
    qdb_option_cluster_disable_auto_compaction(qdb_handle_t handle);


    //! EXPERIMENTAL. Use at your own risk!
    QDB_API_LINKAGE qdb_error_t
    qdb_option_cluster_tidy_memory(qdb_handle_t handle);

    //! EXPERIMENTAL. Use at your own risk!
    QDB_API_LINKAGE qdb_error_t
    qdb_option_cluster_disable_async_pipelines(qdb_handle_t handle);

    //! EXPERIMENTAL. Use at your own risk!
    QDB_API_LINKAGE qdb_error_t
    qdb_option_cluster_enable_async_pipelines(qdb_handle_t handle);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_OPTION_H */
