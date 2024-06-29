#include "tgmath.h"

#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

mat4_mul_ptr mat4_mul = NULL;

bool _avx_supported() {
    int info[4];
#ifdef _WIN32
    __cpuid(info, 1);
#else
    __cpuid(1, info[0], info[1], info[2], info[3]);
#endif
    return (info[2] & ((int)1 << 28)) != 0;
}

void _tgmath_init() {
    if (_avx_supported()) {
        mat4_mul = _avxmat4_mul;
    }
    else {
      
    }
}
