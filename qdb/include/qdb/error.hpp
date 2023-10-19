#pragma once

#include "error.h"

#ifndef QDB_NO_OUTPUT_OPERATOR

#    include <ios>

template <typename Output>
inline Output & operator<<(Output & os, ::qdb_error_t err)
{
    typename Output::fmtflags old_flags = os.flags(); // save
    os << std::showbase << std::hex << static_cast<int>(err);
    os.flags(old_flags); // restore

    return os;
}

#endif // QDB_NO_OUTPUT_OPERATOR
