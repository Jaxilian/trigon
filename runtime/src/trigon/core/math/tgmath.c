#include "trigon/core/utils/debug.h"
#include "avxmath.h"
#include <stdio.h>

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
        debug_err("SSE math not supported yet!\n");
    }
}

void mat4_print(const char* label, const mat4_t m) {
    printf("%s\n", label);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%f ", m[i * 4 + j]);
        }
        printf("\n");
    }
}