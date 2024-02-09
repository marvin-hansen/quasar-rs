#ifndef QDB_API_DIRECT_H
#define QDB_API_DIRECT_H

//! \file direct.h
//! \defgroup direct Direct node storage access

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

    //! \ingroup client
    //! \typedef qdb_direct_handle_t
    //! \brief An opaque handle to internal API-allocated structures needed for
    //! maintaining a direct connection to a cluster.
    typedef // NOLINT(modernize-use-using)
        struct qdb_direct_handle_internal * qdb_direct_handle_t;

    //! \ingroup direct
    //! \brief Opens a direct connection to a node for use with the direct API.
    //!
    //! The returned handled must be freed with qdb_direct_close. Releasing the
    //! direct handle has no impact on non-direct connections or other direct
    //! handles.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param uri A string representing the node address and port (without the
    //! qdb:// prefix)
    //!
    //! \return A valid qdb_direct_handle_t if successful, a null pointer.
    QDB_API_LINKAGE qdb_direct_handle_t qdb_direct_connect(qdb_handle_t handle,
                                                           const char * uri);

    //! \ingroup direct
    //! \brief Close a direct handle initially opened with qdb_direct_connect.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    QDB_API_LINKAGE void qdb_direct_close(qdb_direct_handle_t handle);

    //! \ingroup direct
    //! \brief Retrieves an entry's content from a quasardb node's local
    //! storage.
    //!
    //! If the entry does not exist, the function will fail and return
    //! \ref qdb_e_alias_not_found.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The function will allocate a buffer large enough to hold the entry's
    //! content. This buffer must be released by the caller with a call to
    //! \ref qdb_release().
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
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
    qdb_direct_blob_get(qdb_direct_handle_t handle,
                        const char * alias,
                        const void ** content,
                        qdb_size_t * content_length);

    //! \ingroup direct
    //! \brief Creates a new entry and sets its content to the provided blob
    //!
    //! If the entry already exists the function will fail and will return
    //! \ref qdb_e_alias_already_exists.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of blobs.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the blob content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time This parameter is currently ignored.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_direct_blob_put(qdb_direct_handle_t handle,
                                                    const char * alias,
                                                    const void * content,
                                                    qdb_size_t content_length,
                                                    qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Creates or updates an entry and sets its content to the provided
    //! blob
    //!
    //! If the entry already exists, the function will modify the entry.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use \ref qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! There is no software-defined limit to the size of blobs.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param content A pointer to the buffer with the blob content
    //!
    //! \param content_length The length of the buffer, in bytes
    //!
    //! \param expiry_time This parameter is currently ignored.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_blob_update(qdb_direct_handle_t handle,
                           const char * alias,
                           const void * content,
                           qdb_size_t content_length,
                           qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Removes an entry from the local node's storage, regardless of its
    //! type.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The call is ACID, regardless of the type of the entry and a transaction
    //! will be created if need be.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_direct_remove(qdb_direct_handle_t handle,
                                                  const char * alias);

    //! \ingroup direct
    //! \brief Creates a new signed 64-bit integer.
    //!
    //! Atomically creates an entry of the given alias and sets it to a
    //! cross-platform signed 64-bit integer.
    //! If the entry already exists, the function returns an error.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry or use qdb_never_expires if you don't want the
    //! entry to expire.
    //!
    //! If you want to create or update an entry use \ref qdb_int_update.
    //!
    //! The value will be correctly translated independently of the endianness
    //! of the client's platform.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer The value to which the integer must be set.
    //!
    //! \param expiry_time This parameter is currently ignored.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_update, \ref qdb_int_get, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_put(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t integer,
                                                   qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Creates or updates a signed 64-bit integer.
    //!
    //! Atomically updates an entry of the given alias to the provided value.
    //! If the entry doesn't exist, it will be created.
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! You can specify an expiry time or use qdb_never_expires if you don't
    //! want the entry to expire.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer The value to which the integer must be set.
    //!
    //! \param expiry_time This parameter is currently ignored.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_get, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_int_update(qdb_direct_handle_t handle,
                          const char * alias,
                          qdb_int_t integer,
                          qdb_time_t expiry_time);

    //! \ingroup direct
    //! \brief Retrieves the value of a signed 64-bit integer.
    //!
    //! Atomically retrieves the value of an existing 64-bit integer.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param integer A pointer to an integer that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_update, \ref qdb_int_add
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_get(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t * integer);

    //! \ingroup direct
    //! \brief Atomically increases or decreases a signed 64-bit integer.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! The specified entry will be atomically increased (or decreased)
    //! according to the given addend value:
    //!  - To increase the value, specify a positive addend
    //!  - To decrease the value, specify a negative addend
    //!
    //! The function return the result of the operation.
    //!
    //! The entry must already exist.
    //! If the entry is not an integer, the function will return
    //! \ref qdb_e_incompatible_type.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the entry.
    //!
    //! \param addend The value to add to the existing value.
    //!
    //! \param result A pointer to an integer that will receive the current
    //! value of the entry, if successful.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_int_put, \ref qdb_int_update, \ref qdb_int_get
    QDB_API_LINKAGE qdb_error_t qdb_direct_int_add(qdb_direct_handle_t handle,
                                                   const char * alias,
                                                   qdb_int_t addend,
                                                   qdb_int_t * result);

    //! \ingroup prefix
    //! \brief Retrieves the list of all entries matching the provided prefix.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! A prefix-based search will enable you to find all entries matching a
    //! provided \p prefix.
    //!
    //! This function returns the list of aliases. It's up to the user to query
    //! the content associated with every entry, if needed.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param prefix A pointer to a null-terminated UTF-8 string representing
    //! the desired prefix
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
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_prefix_get(qdb_direct_handle_t handle,
                          const char * prefix,
                          qdb_int_t max_count,
                          const char *** results,
                          size_t * result_count);

    //! \ingroup backup
    //! \brief Backup modification flags.
    typedef enum qdb_backup_flags_t // NOLINT(modernize-use-using)
    {
        qdb_backup_checkpoint = 1, //!< Make checkpoint backup.
        qdb_backup_checkpoint_trim =
            2, //!< Control number of checkpoints in backup.
        qdb_backup_full =
            4, //!< Make full backup. Incompatible with checkpoint flags.
    } qdb_backup_flags_t;

    //! \ingroup backup
    //! \brief Backup additional parameters.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The backup modification flags. Combination of \ref
        //! qdb_backup_flags_t elements.
        qdb_uint_t flags;
        //! Maximal number of checkpoints stored in backup worked with \ref
        //! qdb_backup_checkpoint_trim flag.
        qdb_size_t increments_limit;
    } qdb_backup_options_t;

    //! \ingroup backup
    //! \brief Run backup process on the node.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! Triggers a backup of the node into the specified directory.
    //! The function returns right after the backup is started and will not wait
    //! for its completion. Only one backup per node can be running at any point
    //! in time. Returns an error if a backup is already in progress.
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param output_directory A pointer to a null-terminated UTF-8 string
    //! representing the target backup path on the node size. Can't be null or
    //! empty.
    //!
    //! \param options Additional backup parameters (optional).
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_node_backup(qdb_direct_handle_t handle,
                           const char * output_directory,
                           const qdb_backup_options_t * options);

    //! \ingroup backup
    //! \brief Get the backup progress.
    //!
    //! This function returns last backup progress.
    //! \see \ref qdb_direct_node_backup
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param completed An actually backupped part in Bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_node_get_backup_progress(qdb_direct_handle_t handle,
                                        qdb_uint_t * completed);

    //! \ingroup backup
    //! \brief Abort the running backup.
    //!
    //! This function aborts the last active backup.
    //! \see \ref qdb_direct_node_backup
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_node_abort_backup(qdb_direct_handle_t handle);

    //! \ingroup backup
    //! \brief Backup S3 additional parameters.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! Number of threads used for backup.
        //! If 0, then one thread is used.
        qdb_uint_t thread_count;

        //! Flush memory tables before backup operation.
        //! Default 0 (false).
        int flush_memtable;
    } qdb_backup_s3_options_t;

    //! \ingroup backup
    //! \brief Run backup process to S3 on the node.
    //!
    //! This function bypasses the clustering mechanism and accesses the node
    //! local storage. Entries in the local node storage are not accessible via
    //! the regular API and vice versa.
    //!
    //! Triggers a backup of the node into the S3 cloud.
    //! The function returns right after the backup is started and will not wait
    //! for its completion. Only one backup per node can be running at any point
    //! in time. Returns an error if a backup is already in progress.
    //! \see \ref qdb_direct_node_get_s3_backup_progress
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param bucket A pointer to a null-terminated UTF-8 string
    //! representing the bucket name.
    //!
    //! \param path_prefix A pointer to a null-terminated UTF-8 string
    //! representing the object path prefix.
    //!
    //! \param region A pointer to a null-terminated UTF-8 string
    //! representing the region (optional).
    //!
    //! \param options Additional backup parameters (optional).
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_node_s3_backup(qdb_direct_handle_t handle,
                              const char * bucket,
                              const char * path_prefix,
                              const char * region,
                              const qdb_backup_s3_options_t * options);

    //! \ingroup backup
    //! \brief Backup progress details.
    typedef struct // NOLINT(modernize-use-using)
    {
        //! Total number of files to checkpoint
        qdb_uint_t total;

        //! Number of files completed, guaranteed to be <= total
        qdb_uint_t completed;
    } qdb_backup_progress_t;

    //! \ingroup backup
    //! \brief Get the S3 backup progress.
    //!
    //! This function returns last backup progress.
    //! \see \ref qdb_direct_node_s3_backup
    //!
    //! \param handle A valid handle previously initialized by \ref
    //! qdb_direct_connect
    //!
    //! \param progress A backup progress details.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_direct_node_get_s3_backup_progress(qdb_direct_handle_t handle,
                                           qdb_backup_progress_t * progress);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
