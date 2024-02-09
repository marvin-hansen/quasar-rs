#ifndef QDB_API_LOG_H
#define QDB_API_LOG_H

//! \file log.h
//! \defgroup log Logging-related operations

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

#include "error.h"
#include <stddef.h> // NOLINT(modernize-deprecated-headers)

#ifdef __cplusplus
extern "C"
{
#endif

    //! \ingroup log
    //! \typedef qdb_log_level_t
    //! \brief An enumeration of log level
    //! \see \ref qdb_open
    //! \see \ref qdb_log_callback
    typedef enum qdb_log_level_t // NOLINT(modernize-use-using)
    {
        //! Log everything. Very verbose.
        qdb_log_detailed = 100,
        //! Log debug level messages and below
        qdb_log_debug = 200,
        //! Log information level messages and below
        qdb_log_info = 300,
        //! Log warning level messages and below
        qdb_log_warning = 400,
        //! Log error level messages and below
        qdb_log_error = 500,
        //! Log panic level messages and below. Very terse.
        qdb_log_panic = 600
    } qdb_log_level_t;

    //! \ingroup log
    //! \typedef qdb_log_callback_id
    //! \brief An unique callback identifier returned by \ref
    //! qdb_log_add_callback
    //!
    //! \see \ref qdb_log_add_callback, \ref qdb_log_remove_callback
    typedef size_t qdb_log_callback_id; // NOLINT(modernize-use-using)

    //! \ingroup log
    //! \typedef qdb_log_callback
    //! \brief A typedef representing a log callback
    //!
    //! \see \ref qdb_log_add_callback, \ref qdb_log_remove_callback
    typedef void (*qdb_log_callback)( // NOLINT(modernize-use-using)
        qdb_log_level_t,              /* log level */
        const unsigned long *,        // NOLINT(google-runtime-int)
        /* [year, month, day, hours,
                                  minutes, seconds] (valid only
                                  in the context of the callback)
                                  */
        unsigned long, // NOLINT(google-runtime-int)
        /* process id */
        unsigned long, // NOLINT(google-runtime-int)
        /* thread id */
        const char *, /* message buffer (valid only in the
                         context of the callback) */
        size_t        /* message buffer size */
    );

    //! \ingroup log
    //! \brief Adds a callback that will receive internal API log messages. This
    //! can be used to incorporate internal API statuses in your log.
    //!
    //! \param cb A callback of type \ref qdb_log_callback. The callback will be
    //! called every time the API emits a log message.
    //!
    //! \param callback_id A pointer to a unique callback identifier that can
    //! be used to remove the callback.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_log_callback, \ref qdb_log_remove_callback

    QDB_API_LINKAGE qdb_error_t
    qdb_log_add_callback(qdb_log_callback cb,
                         qdb_log_callback_id * callback_id);

    //! \ingroup log
    //! \brief Removes a previously added log callback.
    //!
    //! \param callback_id The identifier set by \ref qdb_log_add_callback that
    //! will determine which callback to remove.
    //!
    //! \return A \ref qdb_error_t code indicating success or failure.
    //!
    //! \see \ref qdb_log_callback, \ref qdb_log_add_callback
    QDB_API_LINKAGE qdb_error_t
    qdb_log_remove_callback(qdb_log_callback_id callback_id);

    //! \ingroup log
    //! \brief Indicates if logs are synchronized for each API call (disabled by
    //! default).
    //!
    //! \return 1 if logs are flushed before returning from each API call, 0
    //!  otherwise.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE int qdb_log_option_is_sync(void);

    //! \ingroup log
    //! \brief Set logs to be synchronous for each API call (disabled by
    //! default).
    //!
    //! \param sync_logger 1 if logs must be flushed before returning from each
    //! API call, 0
    //!  otherwise.
    //!
    //! \warning This function is still under development. Performance and
    //! compatibility are not guaranteed.
    QDB_API_LINKAGE void qdb_log_option_set_sync(int sync_logger);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QDB_API_LOG_H */
