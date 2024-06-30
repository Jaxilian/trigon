#include "tgmath.h"
#include <memory.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

// mat4_t is defined as float[16], uses column major (Vulkan)

void _avxmat4_mul(mat4_t r, const mat4_t a, const mat4_t b) {
    for (int i = 0; i < 4; i++) {
        __m128 col = _mm_setzero_ps();
        for (int j = 0; j < 4; j++) {
            __m128 b_broadcast = _mm_set1_ps(b[i * 4 + j]);
            __m128 a_col = _mm_loadu_ps(&a[j * 4]);
            col = _mm_add_ps(col, _mm_mul_ps(b_broadcast, a_col));
        }
        _mm_storeu_ps(&r[i * 4], col);
    }
}


void _avxmat4_setpos(mat4_t r, const vec3_t t) {
    r[12] = t[0];
    r[13] = t[1];
    r[14] = t[2];
}

void _avxmat4_translate(mat4_t r, const vec3_t t) {
    mat4_t m = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t[0], t[1], t[2], 1
    };

    mat4_t result;
    _avxmat4_mul(result, m, r);

    memcpy(r, result, sizeof(mat4_t));
}

void _avxmath_test() {
    mat4_t mat1 = mat4_identity;
    mat4_t mat2 = mat4_identity;

    _avxmat4_setpos(mat1, (vec3_t) { 2, 5, 15 });
    _avxmat4_setpos(mat2, (vec3_t) { -12, 1, 512 });

    mat4_print("setpos:", mat1);
    mat4_print("setpos:", mat2);

    _avxmat4_translate(mat1, (vec3_t) { 6, 12, 0 });
    mat4_print("translate:", mat1);
}

