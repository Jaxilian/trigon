#include "tgmath.h"

#include <immintrin.h> 
#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

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

}