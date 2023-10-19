#ifndef QDB_API_TS_H
#define QDB_API_TS_H

//! \file ts.h
//! \defgroup ts Time series operations
//! \warning Time series are under development. Performance and compatibility
//! are not guaranteed.

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
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push)
#pragma pack(8)

    //! \ingroup ts
    //! \brief A timestamped data with a double-precision floating-point value.
    typedef struct
    {
        //! \brief Timestamp.
        qdb_timespec_t timestamp;
        //! \brief Numeric value.
        double value;
    } qdb_ts_double_point;

    //! \ingroup ts
    //! \def QDB_IS_NULL_DOUBLE
    //! \brief True if and only if the \ref qdb_ts_double_point represents a
    //! null value
#define QDB_IS_NULL_DOUBLE(pt) (isnan(pt.value))

    //! \ingroup ts
    //! \brief A timestamped data with a binary content.
    typedef struct
    {
        //! \copydoc qdb_ts_double_point::timestamp
        qdb_timespec_t timestamp;
        //! Pointer to data.
        const void * content;
        //! Length, in bytes, of the data pointed to by \ref content.
        qdb_size_t content_length;
    } qdb_ts_blob_point;

    //! \ingroup ts
    //! \def QDB_IS_NULL_BLOB
    //! \brief True if and only if the \ref qdb_ts_blob_point represents a null
    //! value
#define QDB_IS_NULL_BLOB(pt) (pt.content_length == 0)

    //! \ingroup ts
    //! \brief A timestamped data with a signed 64-bit integer value.
    typedef struct
    {
        //! \brief Timestamp.
        qdb_timespec_t timestamp;
        //! \brief Numeric value.
        qdb_int_t value;
    } qdb_ts_int64_point;

    //! \ingroup ts
    //! \def QDB_IS_NULL_INT64
    //! \brief True if and only if the \ref qdb_ts_int64_point represents a null
    //! value
#define QDB_IS_NULL_INT64(pt) (pt.value == ((qdb_int_t)0x8000000000000000ll))

    //! \ingroup ts
    //! \brief A timestamped data with a signed 64-bit integer value.
    typedef struct
    {
        //! \brief Timestamp.
        qdb_timespec_t timestamp;
        //! \brief Numeric value.
        qdb_timespec_t value;
    } qdb_ts_timestamp_point;

    //! \ingroup ts
    //! \def QDB_IS_NULL_TIMESTAMP
    //! \brief True if and only if the \ref qdb_ts_timestamp_point represents a
    //! null value
#define QDB_IS_NULL_TIMESTAMP(pt) \
    (pt.value.tv_sec == qdb_min_time && pt.value.tv_nsec == qdb_min_time)

    //! \ingroup ts
    //! \brief A timestamped data with a string content.
    typedef struct
    {
        //! \copydoc qdb_ts_double_point::timestamp
        qdb_timespec_t timestamp;
        //! Pointer to data.
        const char * content;
        //! Length, in bytes, of the data pointed to by \ref content.
        qdb_size_t content_length;
    } qdb_ts_string_point;

    //! \ingroup ts
    //! \def QDB_IS_NULL_STRING
    //! \brief True if and only if the \ref qdb_ts_string_point represents a
    //! null value
#define QDB_IS_NULL_STRING(pt) (pt.content_length == 0)

    //! \ingroup ts
    //! \brief Time interval.
    typedef struct
    {
        //! Beginning of the interval, inclusive.
        qdb_timespec_t begin;
        //! End of the interval, exclusive.
        qdb_timespec_t end;
    } qdb_ts_range_t;

    //! \ingroup ts
    //! \brief Types of aggregations that can be computed on a time series.
    typedef enum qdb_ts_aggregation_type_t
    {
        qdb_agg_first = 0, //!< The first (earliest) data point.
        qdb_agg_last = 1,  //!< The last (latest) data point.
        qdb_agg_min = 2,   //!< The data point with the smallest value.
        qdb_agg_max = 3,   //!< The data point with the largest value.
        qdb_agg_arithmetic_mean = 4, //!< The arithmetic mean (average).
        qdb_agg_harmonic_mean = 5,   //!< The harmonic mean.
        qdb_agg_geometric_mean = 6,  //!< The geometric mean.
        qdb_agg_quadratic_mean = 7,  //!< The quadratic mean (root mean squaer).
        qdb_agg_count = 8,           //!< The number of data points.
        qdb_agg_sum = 9,             //!< The sum of values.
        qdb_agg_sum_of_squares = 10, //!< The sum of squares of values.
        qdb_agg_spread = 11,
        //!< The difference between the maximum value and the minimum value.
        qdb_agg_sample_variance = 12,     //!< The sample variance.
        qdb_agg_sample_stddev = 13,       //!< The sample standard deviation.
        qdb_agg_population_variance = 14, //!< The population variance.
        qdb_agg_population_stddev = 15, //!< The population standard deviation.
        qdb_agg_abs_min = 16,  //!< The value with the smallest absolute value.
        qdb_agg_abs_max = 17,  //!< The value with the largest absolute value.
        qdb_agg_product = 18,  //!< The product.
        qdb_agg_skewness = 19, //!< The skewness (shape parameter).
        qdb_agg_kurtosis = 20, //!< The kurtosis (shape parameter).
        qdb_agg_none = 21,     //!< Scalar (no aggregation).
        qdb_agg_distinct_count = 22,
        //!< The number of data points with distinct values.
        qdb_agg_adjacent_count = 29,
        //!< The number of different adjacent values.
        qdb_agg_sum_of_abs = 32, //!< The sum of absolute values.
        qdb_agg_sum_of_abs_changes =
            33, //!< The sum of absolute changes between consecutive values.
    } qdb_ts_aggregation_type_t;

    //! \ingroup ts
    //! \brief Aggregation input and result for columns of blobs.
    typedef struct
    {
        //! \copydoc qdb_ts_double_aggregation_t::type
        qdb_ts_aggregation_type_t type;
        //! \copydoc qdb_ts_double_aggregation_t::range
        qdb_ts_range_t range;
        //! \copydoc qdb_ts_double_aggregation_t::count
        qdb_size_t count;
        //! \copydoc qdb_ts_double_aggregation_t::result
        qdb_ts_blob_point result;
    } qdb_ts_blob_aggregation_t;

    //! \ingroup ts
    //! \brief Aggregation input and result for columns of numeric
    //! floating-point values.
    typedef struct
    {
        //! The type of the aggregation to perform.
        qdb_ts_aggregation_type_t type;
        //! The time interval on which the aggregation should be executed.
        qdb_ts_range_t range;
        //! \brief If applicable, the number of datapoints on which aggregation
        //! has been computed.
        qdb_size_t count;
        //! \brief The result of the aggregation.
        qdb_ts_double_point result;
    } qdb_ts_double_aggregation_t;

    //! \ingroup ts
    //! \brief Aggregation input and result for columns of numeric integer
    //! values.
    typedef struct
    {
        //! The type of the aggregation to perform.
        qdb_ts_aggregation_type_t type;
        //! The time interval on which the aggregation should be executed.
        qdb_ts_range_t range;
        //! \brief If applicable, the number of datapoints on which aggregation
        //! has been computed.
        qdb_size_t count;
        //! \brief The result of the aggregation.
        qdb_ts_int64_point result;
        double exact_result;
    } qdb_ts_int64_aggregation_t;

    //! \ingroup ts
    //! \brief Aggregation input and result for columns of strings.
    typedef struct
    {
        //! \copydoc qdb_ts_double_aggregation_t::type
        qdb_ts_aggregation_type_t type;
        //! \copydoc qdb_ts_double_aggregation_t::range
        qdb_ts_range_t range;
        //! \copydoc qdb_ts_double_aggregation_t::count
        qdb_size_t count;
        //! \copydoc qdb_ts_double_aggregation_t::result
        qdb_ts_string_point result;
    } qdb_ts_string_aggregation_t;

    //! \ingroup ts
    //! \brief Aggregation input and result for columns of numeric integer
    //! values.
    typedef struct
    {
        //! The type of the aggregation to perform.
        qdb_ts_aggregation_type_t type;
        //! The time interval on which the aggregation should be executed.
        qdb_ts_range_t range;
        //! \brief If applicable, the number of datapoints on which aggregation
        //! has been computed.
        qdb_size_t count;
        //! \brief The result of the aggregation.
        qdb_ts_timestamp_point result;
    } qdb_ts_timestamp_aggregation_t;

    //! \ingroup ts
    //! \brief Types of time series columns.
    typedef enum qdb_ts_column_type_t
    {
        qdb_ts_column_uninitialized = -1,
        qdb_ts_column_double = 0, //!< Column of floating point values.
        qdb_ts_column_blob = 1,   //!< Column of binary data.
        qdb_ts_column_int64 = 2,  //!< Column of signed 64-bit integer values.
        qdb_ts_column_timestamp =
            3,                   //!< Column of nanosecond-precision timestamps.
        qdb_ts_column_string = 4 //!< Column of string data.
    } qdb_ts_column_type_t;

    typedef qdb_uint_t qdb_duration_t;

    //! \ingroup ts
    //! \brief Index of a time series columns. Does not necessarily match the
    //! column position at table creation due to modifications to the table
    //! schema (by adding, renaming or removing columns).
    typedef qdb_uint_t qdb_ts_column_index_t;

#define qdb_d_millisecond ((qdb_duration_t)1)
#define qdb_d_second (qdb_d_millisecond * 1000)
#define qdb_d_minute (qdb_d_second * 60)
#define qdb_d_hour (qdb_d_minute * 60)
#define qdb_d_day (qdb_d_hour * 24)
#define qdb_d_week (qdb_d_day * 7)
#define qdb_d_default_shard_size qdb_d_day
#define qdb_d_max_duration ((qdb_duration_t)0x8000000000000000LL / 1000000)

#define qdb_d_min_shard_size qdb_d_millisecond
#define qdb_d_max_shard_size qdb_d_max_duration

    //! \ingroup ts
    //! \brief Description of a time series column.
    typedef struct
    {
        //! \brief A pointer to a null-terminated UTF-8 string representing the
        //! name of the column.
        const char * name;
        //! The type of the column.
        qdb_ts_column_type_t type;
    } qdb_ts_column_info_t;

    //! \ingroup ts
    //! \brief Description of a batch column info.
    typedef struct
    {
        //! \brief A pointer to a null-terminated UTF-8 string representing the
        //! name of the timeseries.
        const char * timeseries;

        //! \brief A pointer to a null-terminated UTF-8 string representing the
        //! name of the column.
        const char * column;

        //! \brief The number of expected elements in one insertion.
        qdb_size_t elements_count_hint;
    } qdb_ts_batch_column_info_t;

    //! \ingroup ts
    //! \brief Description of a batch column that will not be checked.
    typedef struct
    {
        //! \brief A pointer to a null-terminated UTF-8 string representing the
        //! name of the timeseries.
        const char * timeseries;

        //! \brief A pointer to a null-terminated UTF-8 string representing the
        //! name of the column.
        const char * column;

        //! \brief The column's data type.
        qdb_ts_column_type_t column_type;

        //! \brief The column's position in the timeseries.
        qdb_int_t column_index;

        //! \brief The timeseries's shard size.
        qdb_duration_t shard_size;

        //! \brief The number of expected elements in one insertion.
        qdb_size_t elements_count_hint;
    } qdb_ts_batch_unchecked_column_info_t;

    //! \ingroup ts
    //! \brief A representation of binary data
    typedef struct
    {
        //! Pointer to data.
        const void * content;
        //! Length, in bytes, of the data pointed to by \ref content.
        qdb_size_t content_length;
    } qdb_blob_t;

#if QDB_PLATFORM_32
#    define QDB_BLOB_OFFSETOF_CONTENT 0
#    define QDB_BLOB_OFFSETOF_CONTENT_LENGTH 4
#else
#    define QDB_BLOB_OFFSETOF_CONTENT 0
#    define QDB_BLOB_OFFSETOF_CONTENT_LENGTH 8
#endif

#define qdb_int64_undefined ((qdb_int_t)0x8000000000000000ll)
#define qdb_count_undefined ((qdb_size_t)-1)

    //! \ingroup ts
    //! \brief Column sent to the server in a batch.
    typedef struct
    {
        //! The column name in UTF-8 format.
        qdb_string_t name;

        //! The column data type, how it's stored client-side.
        qdb_ts_column_type_t data_type;

        //! The column content to send.
        union
        {
            const qdb_timespec_t * timestamps;
            const qdb_string_t * strings;
            const qdb_blob_t * blobs;
            const qdb_int_t * ints;
            const double * doubles;
        } data;
    } qdb_exp_batch_push_column_t;

    //! \ingroup ts
    //! \brief Schema of a column sent to the server in a batch.
    typedef struct
    {
        //! The column type, how it's stored server-side.
        qdb_ts_column_type_t column_type;

        //! The column index.
        qdb_ts_column_index_t index;

        //! The column symbol table (for symbol columns).
        qdb_string_t symtable;
    } qdb_exp_batch_push_column_schema_t;

    //! \ingroup ts
    //! \brief Data of a table sent to the server in a batch.
    typedef struct
    {
        //! The number of rows to send.
        qdb_size_t row_count;

        //! The number of columns to send.
        qdb_size_t column_count;

        //! The rows timestamps.
        const qdb_timespec_t * timestamps;

        //! The table columns to send.
        const qdb_exp_batch_push_column_t * columns;
    } qdb_exp_batch_push_table_data_t;

    //! \ingroup ts
    //! \brief Ways of working with duplicated data in \ref qdb_exp_batch_push.
    typedef enum // NOLINT(modernize-use-using)
    {
        //! Standard way of pushing values.
        qdb_exp_batch_option_standard = 0,

        //! Operation works with the duplicated data removing.
        qdb_exp_batch_option_unique = 1,
    } qdb_exp_batch_push_options_t;

    //! \ingroup ts
    //! \brief Data and metadata of a table sent to the server in a batch.
    typedef struct
    {
        //! The table name in UTF-8 format.
        qdb_string_t name;

        //! The table data.
        qdb_exp_batch_push_table_data_t data;

        //! Field used by \ref qdb_exp_batch_push_truncate. The ranges
        //! specifying previous data to erase.
        const qdb_ts_range_t * truncate_ranges;

        //! Field used by \ref qdb_exp_batch_push_truncate. The number of
        //! truncated ranges.
        qdb_size_t truncate_range_count;

        //! Field used for controling work with duplicated data.
        //! Except of \ref qdb_exp_batch_push_truncate mode.
        qdb_exp_batch_push_options_t options;

        //! Field used by \ref qdb_exp_batch_option_unique. The column names
        //! array for duplication check. If NULL then all columns will be
        //! checked.
        qdb_string_t * where_duplicate;

        //! Size of \ref where_duplicate array.
        qdb_size_t where_duplicate_count;
    } qdb_exp_batch_push_table_t;

    //! \ingroup ts
    //! \brief Schema of a table sent to the server in a batch. Indexed by the
    //! table and column names given in \ref qdb_exp_batch_push_table_t.
    typedef struct
    {
        //! The table shard size.
        qdb_duration_t shard_size;

        //! The table columns. The column count is given by the associated \ref
        //! qdb_exp_batch_push_table_t, at data.column_count.
        const qdb_exp_batch_push_column_schema_t * columns;
    } qdb_exp_batch_push_table_schema_t;

    //! \ingroup ts
    //! \brief Ways of pushing data to timeseries with \ref qdb_exp_batch_push.
    typedef enum
    {
        //! Standard way of pushing values, as a fully transactional operation.
        qdb_exp_batch_push_transactional = 0,

        //! Truncate previous data in the given ranges from \ref
        //! qdb_exp_batch_push_table_t during the batch push transaction. Works
        //! otherwise like \ref qdb_exp_batch_push_transactional.
        qdb_exp_batch_push_truncate = 1,

        //! Update timeseries buckets inplace.
        //! Faster than \ref qdb_exp_batch_push_transactional for small
        //! incremental writes. However, does not modify different tables at
        //! once and partial modifications are not rolled back.
        qdb_exp_batch_push_fast = 2,

        //! Sent data are asynchronously updated by the server using \ref
        //! qdb_exp_batch_push_fast strategy. Faster than \ref
        //! qdb_exp_batch_push_fast when multiple clients are modifying the same
        //! tables simultaneously. However, the data are generally not yet
        //! inserted when the call returns.
        qdb_exp_batch_push_async = 3,
    } qdb_exp_batch_push_mode_t;

#pragma pack(pop)

    //! \ingroup ts
    //! \brief Creates a time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param shard_size_ms A duration representing the time-span of a single
    //! shard (bucket) of the time series, in milliseconds.
    //!
    //! \param columns An array of column descriptions that are to be
    //! added to the time series.
    //!
    //! \param column_count The number of columns to add.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_create(qdb_handle_t handle,
                  const char * alias,
                  qdb_uint_t shard_size_ms,
                  const qdb_ts_column_info_t * columns,
                  qdb_size_t column_count);

    //! \ingroup ts
    //! \brief Appends columns to an existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param columns An array of column descriptions that are to be
    //! added to the time series.
    //!
    //! \param column_count The number of columns to append.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_insert_columns(qdb_handle_t handle,
                          const char * alias,
                          const qdb_ts_column_info_t * columns,
                          qdb_size_t column_count);

    //! \ingroup ts
    //! \brief Returns all the columns of a time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param[out] columns A pointer to an array that will contain descriptions
    //! of columns present in the time series.
    //!
    //! \param[out] column_count A pointer to an integer that will receive the
    //! number of columns.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_list_columns(qdb_handle_t handle,
                        const char * alias,
                        qdb_ts_column_info_t ** columns,
                        qdb_size_t * column_count);

    //! \ingroup ts
    //! \brief Returns the shard size of a time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param[out] shard_size shard size of the timeseries, in ms.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_ts_shard_size(qdb_handle_t handle,
                                                  const char * alias,
                                                  qdb_uint_t * shard_size);

    //! \ingroup ts
    //! \brief Inserts blob points in a time series column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_blob_insert(qdb_handle_t handle,
                       const char * alias,
                       const char * column,
                       const qdb_ts_blob_point * values,
                       qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts blob points in a time series and erases given ranges in
    //! the same transaction.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_blob_insert_truncate(qdb_handle_t handle,
                                const char * alias,
                                const char * column,
                                const qdb_ts_range_t * ranges,
                                qdb_size_t range_count,
                                const qdb_ts_blob_point * values,
                                qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts double points in a time series.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_double_insert(qdb_handle_t handle,
                         const char * alias,
                         const char * column,
                         const qdb_ts_double_point * values,
                         qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts double points in a time series and erases given ranges in
    //! the same transaction.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_double_insert_truncate(qdb_handle_t handle,
                                  const char * alias,
                                  const char * column,
                                  const qdb_ts_range_t * ranges,
                                  qdb_size_t range_count,
                                  const qdb_ts_double_point * values,
                                  qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts int64 points in a time series.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_int64_insert(qdb_handle_t handle,
                        const char * alias,
                        const char * column,
                        const qdb_ts_int64_point * values,
                        qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts int64 points in a time series and erases given ranges in
    //! the same transaction.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_int64_insert_truncate(qdb_handle_t handle,
                                 const char * alias,
                                 const char * column,
                                 const qdb_ts_range_t * ranges,
                                 qdb_size_t range_count,
                                 const qdb_ts_int64_point * values,
                                 qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts string points in a time series column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_string_insert(qdb_handle_t handle,
                         const char * alias,
                         const char * column,
                         const qdb_ts_string_point * values,
                         qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts string points in a time series and erases given ranges in
    //! the same transaction.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_string_insert_truncate(qdb_handle_t handle,
                                  const char * alias,
                                  const char * column,
                                  const qdb_ts_range_t * ranges,
                                  qdb_size_t range_count,
                                  const qdb_ts_string_point * values,
                                  qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts timestamp points in a time series.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_timestamp_insert(qdb_handle_t handle,
                            const char * alias,
                            const char * column,
                            const qdb_ts_timestamp_point * values,
                            qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Inserts timestamp points in a time series and erases given ranges
    //! in the same transaction.
    //!
    //! Time series are distributed across the cluster and support efficient
    //! insertion anywhere within the time series as well as efficient lookup
    //! based on time.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param values An array of data points to be inserted.
    //!
    //! \param value_count The number of data points to insert.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_timestamp_insert_truncate(qdb_handle_t handle,
                                     const char * alias,
                                     const char * column,
                                     const qdb_ts_range_t * ranges,
                                     qdb_size_t range_count,
                                     const qdb_ts_timestamp_point * values,
                                     qdb_size_t value_count);

    //! \ingroup ts
    //! \brief Retrieves blobs in the specified range of the time series column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] points A pointer to an array that will contain data points
    //! from all given ranges.
    //!
    //! \param[out] point_count A pointer to an integer that will receive the
    //! number of returned points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_blob_get_ranges(qdb_handle_t handle,
                           const char * alias,
                           const char * column,
                           const qdb_ts_range_t * ranges,
                           qdb_size_t range_count,
                           qdb_ts_blob_point ** points,
                           qdb_size_t * point_count);

    //! \ingroup ts
    //! \brief Retrieves doubles in the specified range of the time series
    //! column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] points A pointer to an array that will contain data points
    //! from all given ranges.
    //!
    //! \param[out] point_count A pointer to an integer that will receive the
    //! number of returned points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_double_get_ranges(qdb_handle_t handle,
                             const char * alias,
                             const char * column,
                             const qdb_ts_range_t * ranges,
                             qdb_size_t range_count,
                             qdb_ts_double_point ** points,
                             qdb_size_t * point_count);

    //! \ingroup ts
    //! \brief Retrieves 64-bit integers in the specified range of the time
    //! series column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] points A pointer to an array that will contain data points
    //! from all given ranges.
    //!
    //! \param[out] point_count A pointer to an integer that will receive the
    //! number of returned points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_int64_get_ranges(qdb_handle_t handle,
                            const char * alias,
                            const char * column,
                            const qdb_ts_range_t * ranges,
                            qdb_size_t range_count,
                            qdb_ts_int64_point ** points,
                            qdb_size_t * point_count);

    //! \ingroup ts
    //! \brief Retrieves strings in the specified range of the time series
    //! column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] points A pointer to an array that will contain data points
    //! from all given ranges.
    //!
    //! \param[out] point_count A pointer to an integer that will receive the
    //! number of returned points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_string_get_ranges(qdb_handle_t handle,
                             const char * alias,
                             const char * column,
                             const qdb_ts_range_t * ranges,
                             qdb_size_t range_count,
                             qdb_ts_string_point ** points,
                             qdb_size_t * point_count);

    //! \ingroup ts
    //! \brief Retrieves timestamps in the specified range of the time
    //! series column.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] points A pointer to an array that will contain data points
    //! from all given ranges.
    //!
    //! \param[out] point_count A pointer to an integer that will receive the
    //! number of returned points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_timestamp_get_ranges(qdb_handle_t handle,
                                const char * alias,
                                const char * column,
                                const qdb_ts_range_t * ranges,
                                qdb_size_t range_count,
                                qdb_ts_timestamp_point ** points,
                                qdb_size_t * point_count);

    //! \ingroup ts
    //! \brief Aggregate a sub-part of a blob column of the time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param[in,out] aggregations An array of structures representing the
    //! aggregations to compute and their results.
    //!
    //! \param aggregation_count The number of elements in the aggregations
    //! array.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_ts_aggregation_type_t
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_blob_aggregate(qdb_handle_t handle,
                          const char * alias,
                          const char * column,
                          qdb_ts_blob_aggregation_t * aggregations,
                          qdb_size_t aggregation_count);

    //! \ingroup ts
    //! \brief Aggregate a sub-part of the double column of the time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param[in,out] aggregations An array of structures representing the
    //! aggregations to compute and their results.
    //!
    //! \param aggregation_count The number of elements in the aggregations
    //! array.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_ts_aggregation_type_t
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_double_aggregate(qdb_handle_t handle,
                            const char * alias,
                            const char * column,
                            qdb_ts_double_aggregation_t * aggregations,
                            qdb_size_t aggregation_count);

    //! \ingroup ts
    //! \brief Aggregate a sub-part of the int64 column of the time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param[in,out] aggregations An array of structures representing the
    //! aggregations to compute and their results.
    //!
    //! \param aggregation_count The number of elements in the aggregations
    //! array.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_ts_aggregation_type_t
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_int64_aggregate(qdb_handle_t handle,
                           const char * alias,
                           const char * column,
                           qdb_ts_int64_aggregation_t * aggregations,
                           qdb_size_t aggregation_count);

    //! \ingroup ts
    //! \brief Aggregate a sub-part of a string column of the time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param[in,out] aggregations An array of structures representing the
    //! aggregations to compute and their results.
    //!
    //! \param aggregation_count The number of elements in the aggregations
    //! array.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_ts_aggregation_type_t
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_string_aggregate(qdb_handle_t handle,
                            const char * alias,
                            const char * column,
                            qdb_ts_string_aggregation_t * aggregations,
                            qdb_size_t aggregation_count);

    //! \ingroup ts
    //! \brief Aggregate a sub-part of the timestamp column of the time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param[in,out] aggregations An array of structures representing the
    //! aggregations to compute and their results.
    //!
    //! \param aggregation_count The number of elements in the aggregations
    //! array.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_ts_aggregation_type_t
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_timestamp_aggregate(qdb_handle_t handle,
                               const char * alias,
                               const char * column,
                               qdb_ts_timestamp_aggregation_t * aggregations,
                               qdb_size_t aggregation_count);

    //! \ingroup ts
    //! \brief Retrieves the timestamps, not the values, for a given timeseries
    //! column or the aggregation of all columns.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on. If this pointer is NULL, all columns
    //! will be aggregated in a single range.
    //!
    //! \param ranges An array of ranges (intervals) for which timestamps
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] timestamps A pointer to an array that will contain
    //! timestamps from all given ranges.
    //!
    //! \param[out] timestamp_count A pointer to an integer that will receive
    //! the number of returned timestamps.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_get_timestamps(qdb_handle_t handle,
                          const char * alias,
                          const char * column,
                          const qdb_ts_range_t * ranges,
                          qdb_size_t range_count,
                          qdb_timespec_t ** timestamps,
                          qdb_size_t * timestamp_count);

    //! \ingroup ts
    //! \brief Erase all points in the specified range (left inclusive).
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param column A pointer to a null-terminated UTF-8 string representing
    //! the name of the column to work on.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \param[out] erased_count A pointer to an integer that will receive the
    //!                          number of erased points.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_erase_ranges(qdb_handle_t handle,
                        const char * alias,
                        const char * column,
                        const qdb_ts_range_t * ranges,
                        qdb_size_t range_count,
                        qdb_uint_t * erased_count);

    //! \ingroup ts
    //! \typedef qdb_local_table_t
    //! \brief An opaque local table structure used for bulk insertions
    typedef struct qdb_local_table_internal * qdb_local_table_t;

    //! \ingroup ts
    //! \brief Initialize a local table for bulk insertion in time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param columns An array of column descriptions to which
    //! data will be inserted.
    //!
    //! \param column_count The number of columns.
    //!
    //! \param[out] table A pointer to a local table that will be initialized if
    //! the function returns successfully.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_local_table_init(qdb_handle_t handle,
                            const char * alias,
                            const qdb_ts_column_info_t * columns,
                            qdb_size_t column_count,
                            qdb_local_table_t * table);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of doubles indicated
    //! by the given index.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of
    //! columns in the call to \ref qdb_ts_local_table_init used to
    //! initialize table.
    //!
    //! \param value A value to be set in the chosen column in the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t qdb_ts_row_set_double(qdb_local_table_t table,
                                                      qdb_size_t column_index,
                                                      double value);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of integers indicated
    //! by the given index.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of
    //! columns in the call to \ref qdb_ts_local_table_init used to
    //! initialize table.
    //!
    //! \param value A value to be set in the chosen column in the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_row_set_int64(qdb_local_table_t table,
                                     qdb_size_t column_index,
                                     qdb_int_t value);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of timestamps
    //! indicated by the given index.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of
    //! columns in the call to \ref qdb_ts_local_table_init used to
    //! initialize table.
    //!
    //! \param value A pointer to the value to be set in the chosen column in
    //! the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_row_set_timestamp(qdb_local_table_t table,
                                         qdb_size_t column_index,
                                         const qdb_timespec_t * value);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of blobs indicated by
    //!        the given index, copy data.
    //!
    //! The data will be copied and kept alive until the following \ref
    //! qdb_ts_push call returns.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of columns in the call to
    //! \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t qdb_ts_row_set_blob(qdb_local_table_t table,
                                                    qdb_size_t column_index,
                                                    const void * content,
                                                    qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of blobs indicated by
    //!        the given index without copying data.
    //!
    //! The data will NOT be copied and the user must ensure that the buffer
    //! pointed by \p content is kept alive until the end of the corresponding
    //! call to \ref qdb_ts_push.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of columns in the call to
    //! \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row. The pointed data must be kept alive until the following
    //! \ref qdb_ts_push call returns.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_set_blob_no_copy(qdb_local_table_t table,
                                qdb_size_t column_index,
                                const void * content,
                                qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of strings indicated
    //! by the given index, copy data.
    //!
    //! The data will be copied and kept alive until the following \ref
    //! qdb_ts_push call returns.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of columns in the call to
    //! \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_set_string(qdb_local_table_t table,
                          qdb_size_t column_index,
                          const char * content,
                          qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of strings indicated
    //! by the given index without copying data.
    //!
    //! The data will NOT be copied and the user must ensure that the buffer
    //! pointed by \p content is kept alive until the end of the corresponding
    //! call to \ref qdb_ts_push.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column in which the value
    //! should be set. The index is based on the order of columns in the call to
    //! \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row. The pointed data must be kept alive until the following
    //! \ref qdb_ts_push call returns.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_set_string_no_copy(qdb_local_table_t table,
                                  qdb_size_t column_index,
                                  const char * content,
                                  qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Append current row to the outcoming data buffer.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param timestamp The timestamp of all data points in the row to append.
    //!
    //! \param[out] row_index A pointer to an integer that will receive the
    //! index of the currently appended row. The index is counted since the call
    //! to \ref qdb_ts_local_table_init. The pointer may be null.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_table_row_append(qdb_local_table_t table,
                            const qdb_timespec_t * timestamp,
                            qdb_size_t * row_index);

    //! \ingroup ts
    //! \brief Push all the appended rows to the database.
    //!
    //! All the rows that were appended using qdb_ts_table_row_append since the
    //! last call to \ref qdb_ts_push or \ref qdb_ts_local_table_init will be
    //! pushed.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t qdb_ts_push(qdb_local_table_t table);

    //! \ingroup ts
    //! \brief Retrieves blob data from the current row at the specified
    //! column.
    //!
    //! The retrieved data will be the copy of the internal data stored in \ref
    //! qdb_local_table_t, so the user must ensure to call \ref qdb_release when
    //! she/he no longer needs it in order to clean up the memory.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \param[out] content A non-null pointer to a variable that will hold the
    //! retrieved data.
    //!
    //! \param[out] content_length A non-null pointer to a variable that will
    //! hold the size of the retrieved data.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_get_blob(qdb_local_table_t table,
                        qdb_size_t column_index,
                        const void ** content,
                        qdb_size_t * content_length);

    //! \ingroup ts
    //! \brief Retrieves blob data from the current row at the specified
    //! column without performing copy of data internal to data.
    //!
    //! The retrieved data will be valid only until the next call of \ref
    //! qdb_ts_table_get_ranges on the same \ref qdb_local_table_t or until the
    //! deallocation thereof. If you want to keep the retrieved data for a
    //! longer time, you may call \ref qdb_ts_row_get_blob instead or copy the
    //! data yourself.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \param[out] content A non-null pointer to a variable that will hold the
    //! retrieved data.
    //!
    //! \param[out] content_length A non-null pointer to a variable that will
    //! hold the size of the retrieved data.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    //!
    //! \see \ref qdb_ts_table_get_ranges
    //! \see \ref qdb_ts_row_get_blob
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_get_blob_no_copy(qdb_local_table_t table,
                                qdb_size_t column_index,
                                const void ** content,
                                qdb_size_t * content_length);

    //! \ingroup ts
    //! \brief Retrieves string data from the current row at the specified
    //! column.
    //!
    //! The retrieved data will be the copy of the internal data stored in \ref
    //! qdb_local_table_t, so the user must ensure to call \ref qdb_release when
    //! she/he no longer needs it in order to clean up the memory.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \param[out] content A non-null pointer to a variable that will hold the
    //! retrieved data.
    //!
    //! \param[out] content_length A non-null pointer to a variable that will
    //! hold the size of the retrieved data.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_get_string(qdb_local_table_t table,
                          qdb_size_t column_index,
                          const char ** content,
                          qdb_size_t * content_length);

    //! \ingroup ts
    //! \brief Retrieves string data from the current row at the specified
    //! column without performing copy of data internal to data.
    //!
    //! The retrieved data will be valid only until the next call of \ref
    //! qdb_ts_table_get_ranges on the same \ref qdb_local_table_t or until the
    //! deallocation thereof. If you want to keep the retrieved data for a
    //! longer time, you may call \ref qdb_ts_row_get_string instead or copy the
    //! data yourself.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \param[out] content A non-null pointer to a variable that will hold the
    //! retrieved data.
    //!
    //! \param[out] content_length A non-null pointer to a variable that will
    //! hold the size of the retrieved data.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    //!
    //! \see \ref qdb_ts_table_get_ranges
    //! \see \ref qdb_ts_row_get_string
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_row_get_string_no_copy(qdb_local_table_t table,
                                  qdb_size_t column_index,
                                  const char ** content,
                                  qdb_size_t * content_length);

    //! \ingroup ts
    //! \brief Retrieves double value from the current row at the specified
    //! column.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param[out] value A non-null pointer to a double that will hold the
    //! retrieved value.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t qdb_ts_row_get_double(qdb_local_table_t table,
                                                      qdb_size_t column_index,
                                                      double * value);

    //! \ingroup ts
    //! \brief Retrieves signed 64-bit integer value from the current row at the
    //! specified column.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param[out] value A non-null pointer to a signed 64-bit integer that
    //! will hold the retrieved value.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_row_get_int64(qdb_local_table_t table,
                                     qdb_size_t column_index,
                                     qdb_int_t * value);

    //! \ingroup ts
    //! \brief Retrieves timestamp value from the current row at the specified
    //! column.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param column_index A zero-based index of the column from which the
    //! value should be retrieved. The index is based on the order of columns in
    //! the call to \ref qdb_ts_local_table_init used to initialize table.
    //!
    //! \param[out] value A non-null pointer to a timestamp that will hold the
    //! retrieved value.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_invalid_iterator when \ref qdb_ts_table_get_ranges has not
    //! been called before calling this function.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_row_get_timestamp(qdb_local_table_t table,
                                         qdb_size_t column_index,
                                         qdb_timespec_t * value);

    //! \ingroup ts
    //! \brief Fetch data and advance to the next row.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param[out] timestamp A non-null pointer to a variable that will hold a
    //! timestamp for the fetched row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_iterator_end when there are no more rows in the specified
    //! ranges.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_table_next_row(qdb_local_table_t table, qdb_timespec_t * timestamp);

    //! \ingroup ts
    //! \brief Retrieve all the specified rows from the database.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_iterator_end when there are no more rows in the specified
    //! ranges.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_table_get_ranges(qdb_local_table_t table,
                            const qdb_ts_range_t * ranges,
                            qdb_size_t range_count);

    //! \ingroup ts
    //! \brief Retrieve shard by shard the specified rows from the database.
    //! The table is then seen as it was then this function was called.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_local_table_init.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be retrieved.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure. Returns
    //! \ref qdb_e_iterator_end when there are no more rows in the specified
    //! ranges.
    //!
    //! \warning This function is still under development. Performance and
    //!          compatibility are not guaranteed.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_table_stream_ranges(qdb_local_table_t table,
                               const qdb_ts_range_t * ranges,
                               qdb_size_t range_count);

    //! \ingroup ts
    //! \typedef qdb_batch_table_t
    //! \brief An opaque batch table structure used for bulk insertions
    typedef struct qdb_batch_table_internal * qdb_batch_table_t;

    //! \ingroup ts
    //! \brief Initialize a batch table for bulk insertion in time series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param columns An array of columns descriptions to which
    //! data will be inserted.
    //!
    //! \param column_count The number of columns.
    //!
    //! \param[out] table A pointer to a batch table that will be initialized if
    //! the function returns successfully.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_table_init(qdb_handle_t handle,
                            const qdb_ts_batch_column_info_t * columns,
                            qdb_size_t column_count,
                            qdb_batch_table_t * table);

    //! \ingroup ts
    //! \brief Initialize a batch table for bulk insertion in time series.
    //!
    //! The given information are assumed to be correct and describing existing
    //! timseries columns.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param columns An array of unchecked columns descriptions to which
    //! data will be inserted.
    //!
    //! \param column_count The number of columns.
    //!
    //! \param[out] table A pointer to a batch table that will be initialized if
    //! the function returns successfully.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_ts_batch_table_unchecked_init(
        qdb_handle_t handle,
        const qdb_ts_batch_unchecked_column_info_t * columns,
        qdb_size_t column_count,
        qdb_batch_table_t * table);

    //! \ingroup ts
    //! \brief Add extra columns to a batch table for bulk insertion in time
    //! series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param columns An array of columns descriptions to which
    //! data will be inserted.
    //!
    //! \param column_count The number of columns.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_table_extra_columns(qdb_batch_table_t table,
                                     const qdb_ts_batch_column_info_t * columns,
                                     qdb_size_t column_count);

    //! \ingroup ts
    //! \brief Add extra columns to a batch table for bulk insertion in time
    //! series.
    //!
    //! It is an error to call this function on a non existing time series.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param columns An array of columns descriptions to which
    //! data will be inserted.
    //!
    //! \param column_count The number of columns.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t qdb_ts_batch_table_unchecked_extra_columns(
        qdb_batch_table_t table,
        const qdb_ts_batch_unchecked_column_info_t * columns,
        qdb_size_t column_count);

    //! \ingroup ts
    //! \brief Start a new row to the outcoming data buffer.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param timestamp The timestamp used while appending values.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_start_row(qdb_batch_table_t table,
                           const qdb_timespec_t * timestamp);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of blobs.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param content A pointer to the data to be set in the chosen column.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_row_set_blob(qdb_batch_table_t table,
                              qdb_size_t index,
                              const void * content,
                              qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of blobs without
    //! copying data.
    //!
    //! The data will NOT be copied and the user must ensure that the buffer
    //! pointed by \p content is kept alive until the end of the corresponding
    //! call to \ref qdb_ts_batch_push.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row. The pointed data must be kept alive until the following
    //! \ref qdb_ts_push call returns.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_row_set_blob_no_copy(qdb_batch_table_t table,
                                      qdb_size_t index,
                                      const void * content,
                                      qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of strings.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param content A pointer to the data to be set in the chosen column.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_row_set_string(qdb_batch_table_t table,
                                qdb_size_t index,
                                const char * content,
                                qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of strings without
    //! copying data.
    //!
    //! The data will NOT be copied and the user must ensure that the buffer
    //! pointed by \p content is kept alive until the end of the corresponding
    //! call to \ref qdb_ts_batch_push.
    //!
    //! \param table A valid local table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param content A pointer to the data to be set in the chosen column in
    //! the current row. The pointed data must be kept alive until the following
    //! \ref qdb_ts_push call returns.
    //!
    //! \param content_length Length, in bytes, of the data pointed to by \p
    //! content.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \note The data pointed to by \p content must be kept alive until the
    //!       following \ref qdb_ts_push call returns.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_row_set_string_no_copy(qdb_batch_table_t table,
                                        qdb_size_t index,
                                        const char * content,
                                        qdb_size_t content_length);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of doubles.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param value A value to be set in the chosen column in the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_row_set_double(qdb_batch_table_t table,
                                qdb_size_t index,
                                double value);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of integers.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param value A value to be set in the chosen column in the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_batch_row_set_int64(qdb_batch_table_t table,
                                           qdb_size_t index,
                                           qdb_int_t value);

    //! \ingroup ts
    //! \brief Set a value in the current row in a column of timestamps.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the value you want to modify.
    //!
    //! \param value A pointer to the value to be set in the chosen column in
    //! the current row.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE
    qdb_error_t qdb_ts_batch_row_set_timestamp(qdb_batch_table_t table,
                                               qdb_size_t index,
                                               const qdb_timespec_t * value);

    //! \ingroup ts
    //! \brief Copy efficiently a buffer that will live until the next push.
    //! Useful to avoid tracking temporary blob or string lifetimes.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param source_buffer A valid pointer to the buffer you want to copy.
    //!
    //! \param source_buffer_size The buffer size.
    //!
    //! \param[out] dest_buffer A pointer to access the copied buffer.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_copy_buffer(qdb_batch_table_t table,
                             const void * source_buffer,
                             qdb_size_t source_buffer_size,
                             const void ** dest_buffer);

    //! \ingroup ts
    //! \brief Resize and get direct access to a blob column
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param capacity A valid capacity to resize to.
    //!
    //! \param timestamp A valid timestamp in the shard you wish to pin.
    //!
    //! \param[out] timeoffsets A pointer to access the time offsets of the
    //! column.
    //!
    //! \param[out] data A pointer to access the blob data of the column.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_pin_blob_column(qdb_batch_table_t table,
                                 qdb_size_t index,
                                 qdb_size_t capacity,
                                 qdb_timespec_t * timestamp,
                                 qdb_time_t ** timeoffsets,
                                 qdb_blob_t ** data);

    //! \ingroup ts
    //! \brief Resize and get direct access to a double column
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param capacity A valid capacity to resize to.
    //!
    //! \param timestamp A valid timestamp in the shard you wish to pin.
    //!
    //! \param[out] timeoffsets A pointer to access the time offsets of the
    //! column.
    //!
    //! \param[out] data A pointer to access the double data of the column.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_pin_double_column(qdb_batch_table_t table,
                                   qdb_size_t index,
                                   qdb_size_t capacity,
                                   qdb_timespec_t * timestamp,
                                   qdb_time_t ** timeoffsets,
                                   double ** data);

    //! \ingroup ts
    //! \brief Resize and get direct access to a int64 column
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param capacity A valid capacity to resize to.
    //!
    //! \param timestamp A valid timestamp in the shard you wish to pin.
    //!
    //! \param[out] timeoffsets A pointer to access the time offsets of the
    //! column.
    //!
    //! \param[out] data A pointer to access the int64 data of the column.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_pin_int64_column(qdb_batch_table_t table,
                                  qdb_size_t index,
                                  qdb_size_t capacity,
                                  qdb_timespec_t * timestamp,
                                  qdb_time_t ** timeoffsets,
                                  qdb_int_t ** data);

    //! \ingroup ts
    //! \brief Resize and get direct access to a string column
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param capacity A valid capacity to resize to.
    //!
    //! \param timestamp A valid timestamp in the shard you wish to pin.
    //!
    //! \param[out] timeoffsets A pointer to access the time offsets of the
    //! column.
    //!
    //! \param[out] data A pointer to access the string data of the column.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_pin_string_column(qdb_batch_table_t table,
                                   qdb_size_t index,
                                   qdb_size_t capacity,
                                   qdb_timespec_t * timestamp,
                                   qdb_time_t ** timeoffsets,
                                   qdb_string_t ** data);

    //! \ingroup ts
    //! \brief Resize and get direct access to a timestamp column
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param capacity A valid capacity to resize to.
    //!
    //! \param timestamp A valid timestamp in the shard you wish to pin.
    //!
    //! \param[out] timeoffsets A pointer to access the time offsets of the
    //! column.
    //!
    //! \param[out] data A pointer to access the timestamp data of the column.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_pin_timestamp_column(qdb_batch_table_t table,
                                      qdb_size_t index,
                                      qdb_size_t capacity,
                                      qdb_timespec_t * timestamp,
                                      qdb_time_t ** timeoffsets,
                                      qdb_timespec_t ** data);

    //! \ingroup ts
    //! \brief Resize the column without invalidating the pointers
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param size A valid size to shrink to.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_shrink_pinned_column(qdb_batch_table_t table,
                                      qdb_size_t index,
                                      qdb_size_t size);

    //! \ingroup ts
    //! \brief Resize the column with no guarantee the pointers will not be
    //! invalidated
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param index A valid index to the column you want to access.
    //!
    //! \param size A valid size to resize to.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_unsafe_resize_pinned_column(qdb_batch_table_t table,
                                             qdb_size_t index,
                                             qdb_size_t size);

    //! \ingroup ts
    //! \brief Release memory of all cached columns. The tables metadata
    //! are kept.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_release_columns_memory(qdb_batch_table_t table);

    //! \ingroup ts
    //! \brief Returns the base time of the bucket given a timestamp and the
    //! shard_size of the timeseries
    //!
    //! \param timestamp A timestamp for which the base time should be computed.
    //!
    //! \param shard_size The shard_size of the timeseries in ms.
    //!
    //! \return A base time of the bucket in milliseconds.
    inline qdb_time_t qdb_ts_bucket_base_time(qdb_timespec_t timestamp,
                                              qdb_duration_t shard_size)
    {
        return (timestamp.tv_sec * 1000 + (timestamp.tv_nsec / 1000000))
               / shard_size * shard_size;
    }

    //! \ingroup ts
    //! \brief Returns the offset of the bucket (from its base time) given a
    //! timestamp and the shard_size of the timeseries
    //!
    //! \param timestamp A timestamp for which the offset should be computed.
    //!
    //! \param shard_size The shard_size of the timeseries in ms.
    //!
    //! \return An offset in nanoseconds.
    inline qdb_time_t qdb_ts_bucket_offset(qdb_timespec_t timestamp,
                                           qdb_duration_t shard_size)
    {
        qdb_time_t pure_ns = timestamp.tv_nsec % 1000000;
        qdb_time_t ns_rest_as_ms = (timestamp.tv_nsec - pure_ns) / 1000000;

        return (((timestamp.tv_sec * 1000 + ns_rest_as_ms) % shard_size)
                * 1000000)
               + pure_ns;
    }

    //! \ingroup ts
    //! \brief Push the current batch table.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_ts_batch_push(qdb_batch_table_t table);

    //! \ingroup ts
    //! \brief Push the current batch table, non transactional.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_ts_batch_push_fast(qdb_batch_table_t table);

    //! \ingroup ts
    //! \brief Push asynchronously the current batch table.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_push_async(qdb_batch_table_t table);

    //! \ingroup ts
    //! \brief Push the current batch table, erasing previously stored points in
    //! the given ranges, transactionally.
    //!
    //! \param table A valid batch table previously initialized by
    //! \ref qdb_ts_batch_table_init.
    //!
    //! \param ranges An array of ranges (intervals) for which data
    //! should be erased.
    //!
    //! \param range_count The number of ranges.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_batch_push_truncate(qdb_batch_table_t table,
                               const qdb_ts_range_t * ranges,
                               qdb_size_t range_count);

    //! \ingroup ts
    //! \brief Push the tables content to the server. Sort given rows if needed.
    //! Write table info if asked.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param tables The tables to send.
    //!
    //! \param table_schemas An optional pointer to the tables schema:
    //!  - if it's null, the tables schema will be retrieved from the server.
    //!  - if the underlying pointer is null, the tables schema will also be
    //!  retrieved from the server, and exported to the given pointer. It must
    //!  then released with \ref qdb_release.
    //!  - it the underlying pointer is not null, it will assume the given
    //!  schema and not verifying it with the server before sending tables
    //!  content.
    //!
    //! \param table_count The number of tables.
    //!
    //! \param mode Specifies how the data are pushed.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_exp_batch_push(qdb_handle_t handle,
                       qdb_exp_batch_push_mode_t mode,
                       const qdb_exp_batch_push_table_t * tables,
                       const qdb_exp_batch_push_table_schema_t ** table_schemas,
                       qdb_size_t table_count);

    //! \ingroup ts
    //! \brief Trim the timeseries, so that it uses approximately the provided
    //! size.
    //!
    //! Trimming is done starting from the oldest entries, so that the most
    //! recent shards are retained in the database.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param alias A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param size The desired disk usage size after the operation, in bytes.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t qdb_ts_expire_by_size(qdb_handle_t handle,
                                                      const char * alias,
                                                      qdb_uint_t size);

    //! \ingroup ts
    //! \brief Remove the given shard.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param reference A pointer to a shard reference.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! @see \ref qdb_ts_bucket_base_time, \ref qdb_ts_bucket_nuke_by_name
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_bucket_nuke_by_id(qdb_handle_t handle, const qdb_id_t * reference);

    //! \ingroup ts
    //! \brief Remove the given shard.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param table A pointer to a null-terminated UTF-8 string representing
    //! the alias of the time series.
    //!
    //! \param shard_base_time The base time of the shard to remove, in
    //! milliseconds.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! @see \ref qdb_ts_bucket_base_time, \ref qdb_ts_bucket_nuke_by_id
    QDB_API_LINKAGE qdb_error_t
    qdb_ts_bucket_nuke_by_name(qdb_handle_t handle,
                               const char * table,
                               qdb_time_t shard_base_time);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
