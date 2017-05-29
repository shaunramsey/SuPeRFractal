#include "conf.h"
#include <errno.h>
#include <cassert>
#include <cstdio>

#if !defined(_WIN32) && !defined(_WIN64)

int fopen_s(FILE **f, const char *name, const char *mode) {
    int ret = 0;
    assert(f);
    *f = fopen(name, mode);
    if (!*f)
        ret = errno;
    return ret;
    }

#endif

