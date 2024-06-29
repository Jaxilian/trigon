#include "tgmath.h"

#include <immintrin.h> 
#include <xmmintrin.h>
#include <emmintrin.h>

void _avxvec3_add(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 rv = _mm_add_ps(av, bv);
    _mm_storeu_ps(r, rv);
}

void _avxvec3_sub(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 rv = _mm_sub_ps(av, bv);
    _mm_storeu_ps(r, rv);
}

void _avxvec3_mul(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 rv = _mm_mul_ps(av, bv);
    _mm_storeu_ps(r, rv);
}

void _avxvec3_div(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 rv = _mm_div_ps(av, bv);
    _mm_storeu_ps(r, rv);
}

void _avxvec3_neg(vec3_t r, const vec3_t v) {
    __m128 vv = _mm_loadu_ps(v);
    __m128 zero = _mm_setzero_ps();
    __m128 rv = _mm_sub_ps(zero, vv);
    _mm_storeu_ps(r, rv);
}

float _avxvec3_dot(const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 rv = _mm_mul_ps(av, bv);
    rv = _mm_hadd_ps(rv, rv);
    rv = _mm_hadd_ps(rv, rv);
    float result;
    _mm_store_ss(&result, rv);
    return result;
}

float _avxvec3_magnitude(const vec3_t v) {
    __m128 vv = _mm_loadu_ps(v);
    __m128 rv = _mm_mul_ps(vv, vv);
    rv = _mm_hadd_ps(rv, rv);
    rv = _mm_hadd_ps(rv, rv);
    float result;
    _mm_store_ss(&result, rv);
    return sqrtf(result);
}

void _avxvec3_normalize(vec3_t r, const vec3_t v) {
    float magnitude = _avxvec3_magnitude(v);
    __m128 vv = _mm_loadu_ps(v);
    __m128 mv = _mm_set1_ps(magnitude);
    __m128 rv = _mm_div_ps(vv, mv);
    _mm_storeu_ps(r, rv);
}

void _avxvec3_cross(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);

    __m128 av_yzx = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 av_zxy = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 bv_yzx = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 bv_zxy = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 result = _mm_sub_ps(
        _mm_mul_ps(av_yzx, bv_zxy),
        _mm_mul_ps(av_zxy, bv_yzx)
    );

    _mm_storeu_ps(r, result);
}

void _avxvec3_project(vec3_t r, const vec3_t a, const vec3_t b) {
    float dot_ab = _avxvec3_dot(a, b);
    float dot_bb = _avxvec3_dot(b, b);
    float scalar = dot_ab / dot_bb;
    __m128 bv = _mm_loadu_ps(b);
    __m128 sv = _mm_set1_ps(scalar);
    __m128 rv = _mm_mul_ps(bv, sv);
    _mm_storeu_ps(r, rv);
}

float _avxvec3_distance(const vec3_t a, const vec3_t b) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 diff = _mm_sub_ps(av, bv);
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    sq_diff = _mm_hadd_ps(sq_diff, sq_diff);
    sq_diff = _mm_hadd_ps(sq_diff, sq_diff);
    float result;
    _mm_store_ss(&result, sq_diff);
    return sqrtf(result);
}

void _avxvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t) {
    __m128 av = _mm_loadu_ps(a);
    __m128 bv = _mm_loadu_ps(b);
    __m128 tv = _mm_set1_ps(t);
    __m128 one_minus = _mm_sub_ps(_mm_set1_ps(1.0f), tv);
    __m128 result = _mm_add_ps(_mm_mul_ps(one_minus, av), _mm_mul_ps(tv, bv));
    _mm_storeu_ps(r, result);
}
