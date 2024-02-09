
#ifndef QDB_API_VERSION_H
#define QDB_API_VERSION_H

//! \file version.h
//! \defgroup version Version operations

/*
 *
 * Official quasardb C API 3.14.1
 * 27761e5bd4bee299a94932278182bfae63b5d54d 2023-12-28 01:42:01 +0700
 *
 * Copyright (c) 2009-2022, quasardb SAS. All rights reserved.
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

// clang-format off
#define QDB_API_VERSION_MAJOR 3
#define QDB_API_VERSION_MINOR 14
#define QDB_API_VERSION_PATCH 1
// clang-format on

#define QDB_MAKE_VERSION(major, minor, patch)                     \
    ((((major) % 100) * 10'000'000) + (((minor) % 100) * 100'000) \
     + ((patch) % 100'000))

//! \ingroup version
//! \def QDB_API_VERSION
//! \brief A number increasing with new versions. Can be compared with
//! 'QDB_MAKE_VERSION(x, y, z)'.
//! \see \ref QDB_MAKE_VERSION
#define QDB_API_VERSION \
    QDB_MAKE_VERSION(   \
        QDB_API_VERSION_MAJOR, QDB_API_VERSION_MINOR, QDB_API_VERSION_PATCH)

#define QDB_API_VERSION_SYMBOL_COLUMN QDB_MAKE_VERSION(3, 11, 0)
#if QDB_API_VERSION >= QDB_API_VERSION_SYMBOL_COLUMN
#define QDB_API_HAS_SYMBOL_COLUMN
#endif

#endif /* QDB_API_VERSION_H */
