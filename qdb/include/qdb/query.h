#ifndef QDB_API_QUERY_H
#define QDB_API_QUERY_H

//! \file query.h
//! \defgroup query Query related functions

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

#include "arrow_abi.h"
#include "ts.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //! \ingroup query
    //! \brief Retrieves all entries' aliases that match the specified find
    //! query.
    //!
    //! For the complete grammar, please refer to the documentation.
    //!
    //! Queries are transactional.
    //!
    //! The complexity of this function is dependent on the complexity of the
    //! query.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param query A pointer to a null-terminated UTF-8 string representing
    //! the query to perform.
    //!
    //! \param[out] aliases A pointer to an array of null-terminated UTF-8
    //! string that will list the aliases of the entries matching the query.
    //!
    //! \param[out] alias_count A pointer to an integer that will receive the
    //! number of returned aliases.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_query_find(qdb_handle_t handle,
                                               const char * query,
                                               const char *** aliases,
                                               size_t * alias_count);

    //! \ingroup query
    //! \brief The type of a result
    typedef enum qdb_query_result_value_type_t
    {
        qdb_query_result_none = -1,
        qdb_query_result_double = 0,
        qdb_query_result_blob = 1,
        qdb_query_result_int64 = 2,
        qdb_query_result_timestamp = 3,
        qdb_query_result_count = 4,
        qdb_query_result_string = 5
    } qdb_query_result_value_type_t;

    /* explicit packing will prevent some incompatibility cases */
#pragma pack(push)
#pragma pack(8)
    //! \ingroup query
    //! \brief A variadic structure holding the result type as well as the
    //! result value
    typedef struct
    {
        qdb_query_result_value_type_t type;

        union
        {
            struct
            {
                double value;
            } double_;

            struct
            {
                qdb_int_t value;
            } int64_;

            struct
            {
                const void * content;
                qdb_size_t content_length;
            } blob;

            struct
            {
                qdb_timespec_t value;
            } timestamp;

            struct
            {
                qdb_size_t value;
            } count;

            struct
            {
                const char * content;
                qdb_size_t content_length;
            } string;
        } payload;
    } qdb_point_result_t;

    //! \ingroup query
    //! \brief Holds the result of a query
    typedef struct
    {
        //! An array of null terminated strings representing the returned
        //! columns
        qdb_string_t * column_names;

        //! The count of returned columns
        qdb_size_t column_count;

        //! A two dimensional array of rows, values are acceded as such:
        //!  rows[row_index][col_index]
        qdb_point_result_t ** rows;

        //! The number of returned rows
        qdb_size_t row_count;

        //! The number of scanned points, for information purposes
        //! The actual number of scanned points may be greater
        qdb_size_t scanned_point_count;

        //! An optional, detailed error message about the query failure
        qdb_string_t error_message;
    } qdb_query_result_t;

#pragma pack(pop)

    //! \ingroup query
    //! \brief Run the provided query and creates a table directory with the
    //! results.
    //!
    //! For the complete grammar, please refer to the documentation.
    //!
    //! Queries are transactional.
    //!
    //! The complexity of this function is dependent on the complexity of the
    //! query.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param query A pointer to a null-terminated UTF-8 string representing
    //! the query to perform.
    //!
    //! \param[out] result A pointer to a pointer of qdb_query_result_t that
    //! will receive the results. It must be released later with qdb_release.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release, \ref qdb_option_set_client_max_parallelism
    QDB_API_LINKAGE qdb_error_t qdb_query(qdb_handle_t handle,
                                          const char * query,
                                          qdb_query_result_t ** result);

    //! \ingroup query
    //! \brief Holds a column of an experimental query
    //!
    //! \warning This structure is still under development. Compatibility is not
    //! guaranteed.
    typedef struct
    {
        //! The column name
        qdb_string_t name;

        //! The column values type
        qdb_ts_column_type_t type;

        //! The column values
        union
        {
            qdb_blob_t * blobs;
            qdb_string_t * strings;
            qdb_timespec_t * timestamps;
            qdb_int_t * ints;
            double * doubles;
        } data;
    } qdb_query_experimental_column_t;

    //! \ingroup query
    //! \brief Holds the result of an experimental query
    //!
    //! \warning This structure is still under development. Compatibility is not
    //! guaranteed.
    typedef struct
    {
        //! An array containing the table columns
        qdb_query_experimental_column_t * columns;

        //! The count of returned columns
        qdb_size_t column_count;

        //! The number of returned rows
        qdb_size_t row_count;
    } qdb_query_experimental_result_t;

    //! \ingroup query
    //! \brief Run the provided query to return a table.
    //!
    //! Currently only supports querying some columns of a timeseries.
    //!
    //! Queries are transactional.
    //!
    //! The complexity of this function is dependent on the complexity of the
    //! query.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param query A pointer to a null-terminated UTF-8 string representing
    //! the query to perform.
    //!
    //! \param[out] result A pointer to a pointer of qdb_query_result_t that
    //! will receive the results. It must be released later with qdb_release.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_query_experimental(qdb_handle_t handle,
                           const char * query,
                           qdb_query_experimental_result_t ** result);

    //! \ingroup query
    //! \brief Holds a column of a query result in Arrow format
    //!
    //! \warning This structure is still under development. Compatibility is not
    //! guaranteed.
    typedef struct
    {
        //! The column name
        qdb_string_t name;

        //! The column type
        struct ArrowSchema schema;

        //! The column content
        struct ArrowArray array;
    } qdb_query_arrow_column_t;

    //! \ingroup query
    //! \brief Holds the result of a query in Arrow format
    //!
    //! \warning This structure is still under development. Compatibility is not
    //! guaranteed.
    typedef struct
    {
        //! An array containing the table columns
        qdb_query_arrow_column_t * columns;

        //! The count of returned columns
        qdb_size_t column_count;

        //! The number of returned rows
        qdb_size_t row_count;
    } qdb_query_arrow_result_t;

    //! \ingroup query
    //! \brief Copies a query result to the Arrow format
    //!
    //! The allocated results have to be released later with \ref qdb_release.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param result A pointer to a query result to clone
    //!
    //! \param[out] result_copy A pointer to a a pointer that will receive
    //! API-allocated results whose content will be a copy of the source results
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_query_to_arrow(qdb_handle_t handle,
                       const qdb_query_result_t * result,
                       qdb_query_arrow_result_t ** result_copy);

    //! \ingroup query
    //! \brief Copies a query result to the Arrow format
    //!
    //! The allocated results have to be released later with \ref qdb_release.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param result A pointer to a query result to clone
    //!
    //! \param[out] result_copy A pointer to a a pointer that will receive
    //! API-allocated results whose content will be a copy of the source results
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_query_experimental_to_arrow(
        qdb_handle_t handle,
        const qdb_query_experimental_result_t * result,
        qdb_query_arrow_result_t ** result_copy);

    //! \ingroup query
    //! \typedef qdb_dedup_handle_t
    //! \brief An opaque handle to internal API-allocated structures needed for
    //! deduplicating query results.
    typedef struct qdb_dedup_handle_internal * qdb_dedup_handle_t;

    //! \ingroup query
    //! \brief Creates a \ref qdb_dedup_handle_t.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param[out] dedup_handle A pointer to a \ref qdb_dedup_handle_t that
    //! will be ready for deduplicating query results. It must be released later
    //! with qdb_release.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_init_query_dedup(qdb_handle_t handle,
                         qdb_dedup_handle_t * dedup_handle);

    //! \ingroup query
    //! \brief Deduplicates query results.
    //!
    //! Rows already seen by previous calls to qdb_query_dedup for the given
    //! deduplication handler will not be included in the deduplicated results.
    //!
    //! \param dedup_handle A valid deduplication handle previously initialized
    //! by \ref qdb_init_query_dedup.
    //!
    //! \param result A pointer to a qdb_query_result_t that holds the results
    //! from a previous call to \ref qdb_query. Results given to a deduplication
    //! filter must all come from the same query, but different ranges or where
    //! clauses are allowed.
    //!
    //! \param[out] dedup_result A pointer to a pointer of qdb_query_result_t
    //! that will receive the deduplicated results. It must be released later
    //! with qdb_release.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_query_dedup(qdb_dedup_handle_t dedup_handle,
                    const qdb_query_result_t * result,
                    qdb_query_result_t ** dedup_result);

    //! \ingroup query
    //! \brief Removes previously seen rows from the given deduplication
    //! handler.
    //!
    //! \param dedup_handle A valid deduplication handle previously initialized
    //! by \ref qdb_init_query_dedup. If the $timestamp special column hasn't
    //! been selected all rows are removed.
    //!
    //! \param range A pointer to a qdb_ts_range_t (interval) for which rows
    //! should be removed.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_query_dedup_prune(qdb_dedup_handle_t dedup_handle,
                          const qdb_ts_range_t * range);

    //! \ingroup query
    //! \typedef qdb_query_cont_handle_t
    //! \brief An opaque handle to internal API-allocated structures needed for
    //! managing a continuous query
    typedef struct qdb_query_cont_internal * qdb_query_cont_handle_t;

    //! \ingroup query
    //! \typedef qdb_query_cont_callback_t
    //! \brief A callback used by qdb_query_continuous.
    //! The first parameter of the callback will receive the context given
    //! when calling qdb_query_continuous
    //! The second parameter is the error result of the operation
    //! The third parameter are the new results for the query since the
    //! callback has been last called
    //! \see qdb_query_continuous
    typedef int (*qdb_query_cont_callback_t)(void *,
                                             qdb_error_t,
                                             const qdb_query_result_t *);

    //! \ingroup query
    //! \brief The continuous query mode
    typedef enum qdb_query_continuous_mode_type_t
    {
        qdb_query_continuous_full = 0,
        qdb_query_continuous_new_values_only = 1
    } qdb_query_continuous_mode_type_t;

    //! \ingroup query
    //! \brief Continuously and efficiently query the server with the given
    //! query
    //!
    //! A continuous query receives results in a callback asynchronously as they
    //! are available on the server. This API uses push notifications from the
    //! server to minimize data exchange and resource usage.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param query A pointer to a null-terminated UTF-8 string representing
    //! the query to perform. Any valid QuasarDB query is supported.
    //!
    //! \param mode The mode of the query. Full will return all values at every
    //! call whereas new values will only deliver updates values.
    //!
    //! \param cb A pointer to a function that will be called when new data is
    //! available
    //!
    //! \param cb_context An opaque pointer to any client managed structure to
    //! pass to the callback. May be null if no context is needed.
    //!
    //! \param[out] cont_handle A handle to the continuous query handle. Updates
    //! to the query will be received until qdb_release is called on this
    //! handle.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_query_continuous(qdb_handle_t handle,
                         const char * query,
                         qdb_query_continuous_mode_type_t mode,
                         qdb_query_cont_callback_t cb,
                         void * cb_context,
                         qdb_query_cont_handle_t * cont_handle);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_QUERY_H */
