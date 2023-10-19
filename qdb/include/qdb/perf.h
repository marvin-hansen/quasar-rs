#ifndef QDB_API_PERF_H
#define QDB_API_PERF_H

//! \file perf.h
//! \defgroup perf Performance related structures and functions

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

    //! \ingroup perf
    //! \brief The possible labels for a performance measurement
    typedef enum qdb_perf_label_t
    {
        qdb_pl_undefined = 0,
        qdb_pl_accepted = 1,
        qdb_pl_received = 2,
        qdb_pl_secured = 3,
        qdb_pl_deserialization_starts = 4,
        qdb_pl_deserialization_ends = 5,
        qdb_pl_entering_chord = 6,
        qdb_pl_processing_starts = 7,
        qdb_pl_dispatch = 8,
        qdb_pl_serialization_starts = 9,
        qdb_pl_serialization_ends = 10,
        qdb_pl_processing_ends = 11,
        qdb_pl_replying = 12,
        qdb_pl_replied = 13,
        qdb_pl_entry_writing_starts = 14,
        qdb_pl_entry_writing_ends = 15,
        qdb_pl_content_reading_starts = 16,
        qdb_pl_content_reading_ends = 17,
        qdb_pl_content_writing_starts = 18,
        qdb_pl_content_writing_ends = 19,
        qdb_pl_directory_reading_starts = 20,
        qdb_pl_directory_reading_ends = 21,
        qdb_pl_directory_writing_starts = 22,
        qdb_pl_directory_writing_ends = 23,
        qdb_pl_entry_trimming_starts = 24,
        qdb_pl_entry_trimming_ends = 25,
        qdb_pl_ts_evaluating_starts = 26,
        qdb_pl_ts_evaluating_ends = 27,
        qdb_pl_ts_bucket_updating_starts = 28,
        qdb_pl_ts_bucket_updating_ends = 29,
        qdb_pl_affix_search_starts = 30,
        qdb_pl_affix_search_ends = 31,
        qdb_pl_eviction_starts = 32,
        qdb_pl_eviction_ends = 33,
        qdb_pl_time_vector_tracker_reading_starts = 34,
        qdb_pl_time_vector_tracker_reading_ends = 35,
        qdb_pl_bucket_reading_starts = 36,
        qdb_pl_bucket_reading_ends = 37,
        qdb_pl_entries_directory_reading_starts = 38,
        qdb_pl_entries_directory_reading_ends = 39,
        qdb_pl_acl_reading_starts = 40,
        qdb_pl_acl_reading_ends = 41,
        qdb_pl_time_vector_reading_starts = 42,
        qdb_pl_time_vector_reading_ends = 43,
        qdb_pl_unknown = 255,
    } qdb_perf_label_t;

    //! \ingroup perf
    //! \struct qdb_perf_measurement_t
    //! \brief A structure representing a performance measurement
    //! database.
    typedef struct
    {
        //! \brief Label of the measurement
        qdb_perf_label_t label;
        //! \brief The number of nanoseconds elapsed since the first measurement
        qdb_time_t elapsed;
    } qdb_perf_measurement_t;

    //! \ingroup perf
    //! \struct qdb_perf_profile_t
    //! \brief A structure representing a continuous list of performance
    //! measurements
    typedef struct
    {
        //! \brief A name for the profile
        qdb_string_t name;
        //! \brief Pointer to an array of measurements
        qdb_perf_measurement_t * measurements;
        //! \brief The total number of measurements
        qdb_size_t count;
    } qdb_perf_profile_t;

    //! \ingroup perf
    //! \brief Gets the list of profiles for the the given handle.
    //!
    //! The profiles that have been extracted will be cleared in the given
    //! handle. This means the next call to this function will return new
    //! profile information only.
    //!
    //! The handle must be connected.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \param profiles A pointer to a pointer of profiles list that will be
    //! allocated by the API and must be release with qdb_release
    //!
    //! \param count A pointer to an integer that will receive the number
    //! of returned profiles.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_release
    QDB_API_LINKAGE qdb_error_t
    qdb_perf_get_profiles(qdb_handle_t handle,
                          qdb_perf_profile_t ** profiles,
                          qdb_size_t * count);

    //! \ingroup perf
    //! \brief Clears all accumulated profile information in the associated
    //! handle
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_perf_clear_all_profiles(qdb_handle_t handle);

    //! \ingroup perf
    //! \brief Enables for all threads client side collection of performance
    //! data sent by the server.
    //!
    //! By default the client will not store performance data returned by the
    //! server.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_perf_enable_client_tracking(qdb_handle_t handle);

    //! \ingroup perf
    //! \brief Disables client side collection of performance data sent by the
    //! server on the provided handle.
    //!
    //! By default the client will not store performance data returned by the
    //! server.
    //!
    //! \param handle A valid handle previously initialized by \ref qdb_open or
    //! \ref qdb_open_tcp.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    QDB_API_LINKAGE qdb_error_t
    qdb_perf_disable_client_tracking(qdb_handle_t handle);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
