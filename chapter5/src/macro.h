#pragma once

#include <string.h>
#include <assert.h>
#include"util.h"

#define YY_ASSERT(x) \
    if(!(x)) { \
        YY_LOG_ERROR(YY_LOG_ROOT()) << "ASSERTION: " << #x \
            << "\nbacktrace:\n"  \
            <<yy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define YY_ASSERT2(x, w)  \
    if(!(x)) { \
        YY_LOG_ERROR(YY_LOG_ROOT()) << "ASSERTION: " << #x \
            << "\n" << w \
            << "\nbacktrace:\n"  \
            <<yy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

