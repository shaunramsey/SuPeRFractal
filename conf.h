#pragma once

#ifndef _super_fractal_8641_h
#define _super_fractal_8641_h

#ifdef __unix
#define nullptr 0
#endif


#ifdef __unix
#include <errno.h>
#include <cassert>
#include <cstdio>

int fopen_s(FILE **f, const char *name, const char *mode);/* {
    int ret = 0;
    assert(f);
    *f = fopen(name, mode);
    if (!*f)
        ret = errno;
    return ret;
    }*/


#endif

#endif
