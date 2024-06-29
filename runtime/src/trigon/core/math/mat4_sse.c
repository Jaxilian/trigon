#include "tgmath.h"

#include <xmmintrin.h>
#include <emmintrin.h>

// mat4_t is = float[16] , uses column major (Vulkan)

void _ssemat4_mul(mat4_t r, const mat4_t a, const mat4_t b) {
    for (int i = 0; i < 4; i++) {
        __m128 row = _mm_setzero_ps();
        for (int j = 0; j < 4; j++) {
            __m128 a_broadcast = _mm_set1_ps(a[i + j * 4]);
            __m128 b_col = _mm_loadu_ps(&b[j * 4]);
            row = _mm_add_ps(row, _mm_mul_ps(a_broadcast, b_col));
        }
        _mm_storeu_ps(&r[i * 4], row);
    }
}