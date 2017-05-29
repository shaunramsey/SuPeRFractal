#pragma once

#ifndef _super_fractal_8641_h
#define _super_fractal_8641_h


#if !defined(_WIN32) && !defined(_WIN64)


#define nullptr 0

#include <cstdio>

int fopen_s(FILE **f, const char *name, const char *mode);/* {
    int ret = 0;
    assert(f);
    *f = fopen(name, mode);
    if (!*f)
        ret = errno;
    return ret;
    }*/


#endif //not win32/64
#endif //for the header
