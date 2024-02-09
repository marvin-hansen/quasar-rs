#ifndef QDB_API_BATCH_H
#define QDB_API_BATCH_H

//! \file batch.h
//! \defgroup batch Batch functions

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

/* explicit packing will prevent some incompatibility cases */
#pragma pack(push)
#pragma pack(8)

    //! \ingroup batch
    //! \typedef qdb_operation_type_t
    //! \brief An enumeration of possible operation type.
    //!
    //! Operations are used by batches and transactions.
    typedef enum qdb_operation_type_t // NOLINT(modernize-use-using)
    {
        //! Unitialized value
        qdb_op_uninitialized = -1,

        //! A blob get operation
        qdb_op_blob_get = 0,
        //! A blob put operation
        qdb_op_blob_put = 1,
        //! A blob update operation
        qdb_op_blob_update = 2,
        //! A blob compare and swap operation
        qdb_op_blob_cas = 4,
        //! A blob get and update
        qdb_op_blob_get_and_update = 5,
        //! A blob remove operation
        qdb_op_blob_remove = 28,

        //! Tag existence test operation
        qdb_op_has_tag = 8,

        //! An integer put operation
        qdb_op_int_put = 9,
        //! An integer update operation
        qdb_op_int_update = 10,
        //! An integer get operation
        qdb_op_int_get = 11,
        //! An integer increase/decrease operation
        qdb_op_int_add = 12,
        //! An integer remove operation
        qdb_op_int_remove = 29,

        //! Entry type check
        qdb_op_get_entry_type = 13,
        //! A value get operation
        qdb_op_value_get = 14,

        //! A double put operation
        qdb_op_double_put = 15,
        //! A double update operation
        qdb_op_double_update = 16,
        //! A double get operation
        qdb_op_double_get = 17,
        //! A double increase/decrease operation
        qdb_op_double_add = 18,
        //! A double remove operation
        qdb_op_double_remove = 30,

        //! A string get operation
        qdb_op_string_get = 19,
        //! A string put operation
        qdb_op_string_put = 20,
        //! A string update operation
        qdb_op_string_update = 21,
        //! A string compare and swap operation
        qdb_op_string_cas = 22,
        //! A string get and update
        qdb_op_string_get_and_update = 23,
        //! A string remove operation
        qdb_op_string_remove = 31,

        //! A timestamp put operation
        qdb_op_timestamp_put = 24,
        //! A timestamp update operation
        qdb_op_timestamp_update = 25,
        //! A timestamp get operation
        qdb_op_timestamp_get = 26,
        //! A timestamp increase/decrease operation
        qdb_op_timestamp_add = 27,
        //! A timestamp remove operation
        qdb_op_timestamp_remove = 32,
        //! An entry trimming operation
        qdb_op_trim_entry = 33,
    } qdb_operation_type_t;

    //! \ingroup batch
    //! \brief The required parameters for an integer operation within a batch
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The value of the integer to use.
        qdb_int_t value;
        //! The optional expiration time. Use \ref qdb_never_expires for no
        //! expiration.
        qdb_time_t expiry_time;
    } int_put_update_t;

    //! \ingroup batch
    //! \brief The required parameters for a blob put or update operations
    //! within a batch
    typedef struct // NOLINT(modernize-use-using)
    {
        //! A pointer to the buffer with the data to be written.
        const void * content;
        //! The size of the buffer pointed by content.
        qdb_size_t content_size;
        //! The optional expiration time. Use \ref qdb_never_expires for no
        //! expiration.
        qdb_time_t expiry_time;
    } blob_put_update_t;

    //! \ingroup batch
    //! \brief The required parameters for a string put or update operations
    //! within a batch
    typedef struct // NOLINT(modernize-use-using)
    {
        //! A pointer to the buffer with the data to be written.
        const char * content;
        //! The size of the buffer pointed by content.
        qdb_size_t content_size;
        //! The optional expiration time. Use \ref qdb_never_expires for no
        //! expiration.
        qdb_time_t expiry_time;
    } string_put_update_t;

    //! \ingroup batch
    //! \brief The required parameters for a double operation within a batch
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The value of the 64-bit floating-point number to use.
        double value;
        //! The optional expiration time. Use \ref qdb_never_expires for no
        //! expiration.
        qdb_time_t expiry_time;
    } double_put_update_t;

    //! \ingroup batch
    //! \brief The required parameters for a timestamp operation within a batch
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The value of the timestamp to use.
        qdb_timespec_t value;
        //! The optional expiration time. Use \ref qdb_never_expires for no
        //! expiration.
        qdb_time_t expiry_time;
    } timestamp_put_update_t;

    //! \ingroup batch
    //! \brief A single operation containing all parameters to execute the
    //! operation in a batch or in a transaction.
    //!
    //! You should initialize operations before usage with the \ref
    //! qdb_init_operations function.
    //!
    typedef struct // NOLINT(modernize-use-using)
    {
        //! The type of the operation
        qdb_operation_type_t type;
        //! A pointer to a null-terminated UTF-8 string representing the
        //! alias of the entry.
        const char * alias;

        //! A \ref qdb_error_t code indicating success or failure after the
        //! operation has been executed
        qdb_error_t error;

        //! Operation specific parameters
        union
        {
            //! Tag specific operation parameters
            struct
            {
                //! A pointer to a null-terminated UTF-8 string representing the
                //! tag to test for existence
                const char * tag;
            } has_tag;

            //! Integer increment/decrement specific operation parameters
            struct
            {
                //! A signed 64-bit integer representing the result of the
                //! operation
                qdb_int_t result;
                //! The value to add or subtract to the entry
                qdb_int_t addend;
            } int_add;

            //! Integer get specific operation parameters
            struct
            {
                //! The result of the integer get operation
                qdb_int_t result;
            } int_get;

            //! Integer put specific operation parameters
            int_put_update_t int_put;
            //! Integer update specific operation parameters
            int_put_update_t int_update;

            //! Double increment/decrement specific operation parameters
            struct
            {
                //! A 64-bit floating-point representing the result of the
                //! operation
                double result;
                //! The value to add or subtract to the entry
                double addend;
            } double_add;

            //! Double get specific operation parameters
            struct
            {
                //! The result of the double get operation
                double result;
            } double_get;

            //! Double put specific operation parameters
            double_put_update_t double_put;
            //! Double update specific operation parameters
            double_put_update_t double_update;

            struct
            {
                //! The result of the get entry type operation
                qdb_entry_type_t type;
            } get_entry_type;

            //! Blob put specific operation parameters
            blob_put_update_t blob_put;
            //! Blob update specific operation parameters
            blob_put_update_t blob_update;

            //! Blob compare and swap specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the blob
                //! operation, if any.
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const void * original_content;
                //! The size of the result buffer.
                qdb_size_t original_content_size;
                //! A pointer to the buffer with the data to be written.
                const void * new_content;
                //! The size of the buffer pointed by content.
                qdb_size_t new_content_size;

                //! A pointer to the comparand.
                const void * comparand;
                //! The size of the buffer pointed by comparand.
                qdb_size_t comparand_size;

                //! Optional offset for the comparand.
                //! Leave to 0 if not needed.
                qdb_size_t comparand_offset;

                //! The optional expiration time. Use \ref qdb_never_expires for
                //! no expiration.
                qdb_time_t expiry_time;
            } blob_cas;

            //! String compare and swap specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the blob
                //! operation, if any.
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const char * original_content;
                //! The size of the result buffer.
                qdb_size_t original_content_size;
                //! A pointer to the buffer with the data to be written.
                const char * new_content;
                //! The size of the buffer pointed by content.
                qdb_size_t new_content_size;

                //! A pointer to the comparand.
                const char * comparand;
                //! The size of the buffer pointed by comparand.
                qdb_size_t comparand_size;

                //! Optional offset for the comparand.
                //! Leave to 0 if not needed.
                qdb_size_t comparand_offset;

                //! The optional expiration time. Use \ref qdb_never_expires for
                //! no expiration.
                qdb_time_t expiry_time;
            } string_cas;

            //! Blob get specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the blob
                //! operation
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const void * content;
                //! The size of the result buffer.
                qdb_size_t content_size;

                //! Optional offset from which the content should be returned.
                //! Leave to 0 if not needed.
                qdb_size_t content_offset;
            } blob_get;

            //! Blob get and update specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the blob
                //! operation
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const void * original_content;
                //! The size of the result buffer.
                qdb_size_t original_content_size;

                //! A pointer to the buffer with the data to be written.
                const void * new_content;
                //! The size of the buffer pointed by content.
                qdb_size_t new_content_size;

                //! The optional expiration time. Use \ref qdb_never_expires for
                //! no expiration.
                qdb_time_t expiry_time;
            } blob_get_and_update;

            //! String get and update specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the blob
                //! operation
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const char * original_content;
                //! The size of the result buffer.
                qdb_size_t original_content_size;

                //! A pointer to the buffer with the data to be written.
                const char * new_content;
                //! The size of the buffer pointed by content.
                qdb_size_t new_content_size;

                //! The optional expiration time. Use \ref qdb_never_expires for
                //! no expiration.
                qdb_time_t expiry_time;
            } string_get_and_update;

            string_put_update_t string_put;
            string_put_update_t string_update;

            //! String get specific operation parameters
            struct
            {
                //! A pointer to the buffer containing the result of the string
                //! operation
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const char * content;
                //! The size of the result buffer.
                qdb_size_t content_size;

                //! Optional offset from which the content should be returned.
                //! Leave to 0 if not needed.
                qdb_size_t content_offset;
            } string_get;

            //! Timestamp get specific operation parameters
            struct
            {
                //! The result of the double get operation
                qdb_timespec_t result;
            } timestamp_get;

            //! Timestamp put specific operation parameters
            timestamp_put_update_t timestamp_put;
            //! Timestamp update specific operation parameters
            timestamp_put_update_t timestamp_update;

            //! Timestamp increment/decrement specific operation parameters
            struct
            {
                //! A timestamp representing the result of the operation
                qdb_timespec_t result;
                //! The value to add or subtract to the entry
                qdb_timespec_t addend;
            } timestamp_add;

            //! Value get specific operation parameters
            struct
            {
                //! The type of the entry returned by the get value operation
                qdb_entry_type_t type;

                //! A pointer to the buffer containing the result of the get
                //! operation when the returned type is blob
                //! This buffer is API-allocated and will be freed by a call to
                //! \ref qdb_release
                const void * blob_content;
                //! The size of the result buffer.
                qdb_size_t blob_content_size;

                //! The result of the get operation when the returned type is
                //! integer
                qdb_int_t int_result;

                //! The result of the get operation when the returned type is
                //! double
                double double_result;

                //! The result of the get operation when the returned type is
                //! timestap
                qdb_timespec_t timestamp_result;
            } value_get;

            //! Trim entry specific operation parameters
            struct
            {
                //! The result of the trimming operation - number of bytes
                //! trimmed
                qdb_uint_t bytes_trimmed;
            } trim_entry;
        };
    } qdb_operation_t;

#pragma pack(pop)

    //! \ingroup batch
    //! \brief Initializes an array of \ref qdb_operation_t
    //!
    //! Use this function to make sure all the fields of the operation structure
    //! are properly initialized to their default values.
    //!
    //! After this function has been called, the user must then set the required
    //! parameter for each operation.
    //!
    //! \param operations A pointer to an array of operations
    //!
    //! \param operation_count The number of elements in the array of operations
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_operation_t
    QDB_API_LINKAGE qdb_error_t
    qdb_init_operations(qdb_operation_t * operations, size_t operation_count);

    //! \ingroup batch
    //! \brief Coalesces an array of \ref qdb_operation_t
    //!
    //! Use this function to minimize operations to a given entry by omiting
    //! redundant operations, combining multiple write operations, and resolving
    //! known read operations.
    //!
    //! After this function has been called, operations in the input array that
    //! were resolved locally will have an error code indicating success. The
    //! remaining input operations will have qdb_e_skipped as error code, and
    //! their effects will be subsumed by operations in the output array of
    //! coalesced operations. All operations are assumed to succeed, if a
    //! coalesced operation fails locally resolved operations on the same entry
    //! are considered invalid.
    //!
    //! This function allocates memory for the array of coalesced operations,
    //! call qdb_release on the coalesced_operations array to release memory.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param operations A pointer to a correctly constructed array of
    //! operations
    //!
    //! \param operation_count The number of elements in the array of operations
    //!
    //! \param coalesced_operations A pointer to the array of coalesced
    //! operations
    //!
    //! \param coalesced_operation_count A pointer to the number of elements in
    //! the array of coalesced operations
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_operation_t, \ref qdb_run_batch, \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_coalesce_operations(qdb_handle_t handle,
                            qdb_operation_t * operations,
                            size_t operation_count,
                            qdb_operation_t ** coalesced_operations,
                            size_t * coalesced_operation_count);

    //! \ingroup batch
    //! \brief Runs the operations in batch.
    //!
    //! When operations are run in batch the order of execution is optimized for
    //! speed and each operation is run independently.
    //!
    //! Running operations is batch is generally the best way to achieve a very
    //! high level of performance, especially for bulk loading of data. That's
    //! because batches minimize network communication and organize the order of
    //! execution for maximum throughput.
    //!
    //! If one or several operations within the batch fail, the rest of the
    //! batch isn't affected.
    //!
    //! If one or several operations within the batch is ill-formed, the whole
    //! batch will be aborted. An ill formed operation is for example an
    //! operation missing a parameter.
    //!
    //! Each operation will result a result according to its type.
    //!
    //! After a batch has been run (successful or not), the user must call
    //! \ref qdb_release on the operations pointer to release all API-allocated
    //! buffers.
    //!
    //! For an ordered,  transactional "all or nothing" behavior use
    //! \ref qdb_run_transaction.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param operations A pointer to a correctly constructed array of
    //! operations
    //!
    //! \param operation_count The number of elements in the array of operations
    //!
    //! \return The number of successful operations in the batch.
    //!
    //! \see \ref qdb_operation_t, \ref qdb_run_transaction, \ref qdb_release
    QDB_API_LINKAGE size_t qdb_run_batch(qdb_handle_t handle,
                                         qdb_operation_t * operations,
                                         size_t operation_count);

    //! \ingroup batch
    //! \brief Runs the operations in a single transaction.
    //!
    //! Operations that run in a transaction will be executed ordered to
    //! preserve the logic of the transaction and if an error occurred the
    //! previously executed operations will be rolled back.
    //!
    //! The operation is committed if and only if all operations have been
    //! successful.
    //!
    //! The result of the transaction is not visible by other clients until it
    //! has been committed.
    //!
    //! After a transaction has been run (successful or not), the user must call
    //! \ref qdb_release on operations to release all API-allocated buffers.
    //!
    //! Use transactions when integrity and order is paramount. For maximum
    //! performances
    //! consider using batches (\ref qdb_run_batch).
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param operations A pointer to a correctly constructed array of
    //! operations
    //!
    //! \param operation_count The number of elements in the array of operations
    //!
    //! \param failure_index A pointer to the index of the first failed
    //! operation
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_operation_t, \ref qdb_run_batch, \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_run_transaction(qdb_handle_t handle,
                        qdb_operation_t * operations,
                        size_t operation_count,
                        size_t * failure_index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_BATCH_H */
