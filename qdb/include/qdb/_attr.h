#ifndef QDB_API_ATTR_H
#define QDB_API_ATTR_H

//! \file _attr.h
//! \defgroup attr Function attributes

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

//! \ingroup attr
//! \def QDB_DEPRECATED
//! \brief Mark a function as deprecated
#ifndef QDB_DEPRECATED
#    ifdef __GNUC__
#        define QDB_DEPRECATED(msg) __attribute__((deprecated(msg)))
#    elif defined(_MSC_VER)
#        define QDB_DEPRECATED(msg) __declspec(deprecated(msg))
#    else
#        pragma message( \
            "WARNING: You need to implement DEPRECATED for this compiler")
#        define QDB_DEPRECATED(msg)
#    endif
#endif

#if defined(QDB_API_DYN_LINK) && defined(QDB_API_STATIC_LINK)
#    error Must not define both QDB_API_DYN_LINK and QDB_API_STATIC_LINK
#endif

// Dynamic link by default.
#if !defined(QDB_API_DYN_LINK) && !defined(QDB_API_STATIC_LINK)
#    define QDB_API_DYN_LINK
#endif

//! \ingroup attr
//! \def QDB_API_LINKAGE
//! \brief Linking parameters for the API functions
#ifndef QDB_API_LINKAGE
#    if defined(QDB_API_DYN_LINK)
#        ifdef _MSC_VER
#            ifdef QDB_API_SOURCE
#                define QDB_API_LINKAGE __declspec(dllexport)
#            else
#                define QDB_API_LINKAGE __declspec(dllimport)
#            endif
#        else
#            ifdef QDB_API_SOURCE
#                define QDB_API_LINKAGE __attribute__((visibility("default")))
#            else
#                define QDB_API_LINKAGE
#            endif
#        endif
#    endif
#endif

#ifndef QDB_API_LINKAGE
#    define QDB_API_LINKAGE
#endif

#endif /* QDB_API_ATTR_H */
