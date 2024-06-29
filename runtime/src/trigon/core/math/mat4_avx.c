#include "tgmath.h"

#include <immintrin.h> 
#include <xmmintrin.h>
#include <emmintrin.h>

// mat4_t is = float[16] , uses column major (Vulkan)

void _avxmat4_mul(mat4_t r, const mat4_t a, const mat4_t b) {
    for (int i = 0; i < 4; i++) {
        __m256 col = _mm256_setzero_ps();
        for (int j = 0; j < 4; j++) {
            __m256 b_broadcast = _mm256_set1_ps(b[i + j * 4]);
            __m256 a_col = _mm256_loadu_ps(&a[j * 4]);
            col = _mm256_add_ps(col, _mm256_mul_ps(b_broadcast, a_col));
        }
        _mm256_storeu_ps(&r[i * 4], col);
    }
}