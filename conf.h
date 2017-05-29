#pragma once
/*Copyright 2017 Shaun Ramsey

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
