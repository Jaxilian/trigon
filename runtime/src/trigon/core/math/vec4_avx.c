#include "tgmath.h"
#include <math.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

static inline __m128 _load_vec4(const vec4_t v) {
    return _mm_loadu_ps(v);
}

static inline void _store_vec4(vec4_t r, __m128 v) {
    _mm_storeu_ps(r, v);
}

void _avxvec4_add(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_add_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_sub(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_sub_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_mul(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_mul_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_div(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_div_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_neg(vec4_t r, const vec4_t v) {
    _store_vec4(r, _mm_sub_ps(_mm_setzero_ps(), _load_vec4(v)));
}

float _avxvec4_dot(const vec4_t a, const vec4_t b) {
    __m128 rv = _mm_dp_ps(_load_vec4(a), _load_vec4(b), 0xFF);
    return _mm_cvtss_f32(rv);
}

float _avxvec4_magnitude(const vec4_t v) {
    __m128 rv = _mm_dp_ps(_load_vec4(v), _load_vec4(v), 0xFF);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec4_normalize(vec4_t r, const vec4_t v) {
    __m128 vv = _load_vec4(v);
    __m128 mv = _mm_set1_ps(_avxvec4_magnitude(v));
    _store_vec4(r, _mm_div_ps(vv, mv));
}

void _avxvec4_cross(vec4_t r, const vec4_t a, const vec4_t b) {
    __m128 av = _load_vec4(a);
    __m128 bv = _load_vec4(b);
    __m128 av_yzx = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 av_zxy = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 bv_yzx = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 bv_zxy = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 result = _mm_sub_ps(_mm_mul_ps(av_yzx, bv_zxy), _mm_mul_ps(av_zxy, bv_yzx));
    result = _mm_blend_ps(result, _mm_setzero_ps(), 0x8);  // Zero the 4th component
    _store_vec4(r, result);
}

void _avxvec4_project(vec4_t r, const vec4_t a, const vec4_t b) {
    float scalar = _avxvec4_dot(a, b) / _avxvec4_dot(b, b);
    _store_vec4(r, _mm_mul_ps(_load_vec4(b), _mm_set1_ps(scalar)));
}

float _avxvec4_distance(const vec4_t a, const vec4_t b) {
    __m128 diff = _mm_sub_ps(_load_vec4(a), _load_vec4(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0xFF);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec4_lerp(vec4_t r, const vec4_t a, const vec4_t b, float t) {
    __m128 av = _load_vec4(a);
    __m128 bv = _load_vec4(b);
    __m128 tv = _mm_set1_ps(t);
    __m128 one_minus = _mm_sub_ps(_mm_set1_ps(1.0f), tv);
    _store_vec4(r, _mm_add_ps(_mm_mul_ps(one_minus, av), _mm_mul_ps(tv, bv)));
}

void _avxvec4_copy(vec4_t r, const vec4_t v) {
    _store_vec4(r, _load_vec4(v));
}

bool _avxvec4_eq(const vec4_t a, const vec4_t b) {
    __m128 cmp = _mm_cmpeq_ps(_load_vec4(a), _load_vec4(b));
    return (_mm_movemask_ps(cmp) & 0xF) == 0xF;
}

void _avxvec4_scalar_mul(vec4_t r, const vec4_t v, float s) {
    _store_vec4(r, _mm_mul_ps(_load_vec4(v), _mm_set1_ps(s)));
}

void _avxvec4_scalar_div(vec4_t r, const vec4_t v, float s) {
    _store_vec4(r, _mm_div_ps(_load_vec4(v), _mm_set1_ps(s)));
}

void _avxvec4_reflect(vec4_t r, const vec4_t v, const vec4_t normal) {
    __m128 nv = _load_vec4(normal);
    __m128 scalar = _mm_set1_ps(2.0f * _avxvec4_dot(v, normal));
    __m128 rv = _mm_sub_ps(_load_vec4(v), _mm_mul_ps(nv, scalar));
    _store_vec4(r, rv);
}

float _avxvec4_angle_between(const vec4_t a, const vec4_t b) {
    return acosf(_avxvec4_dot(a, b) / (_avxvec4_magnitude(a) * _avxvec4_magnitude(b)));
}

void _avxvec4_hadamard(vec4_t r, const vec4_t a, const vec4_t b) {
    _avxvec4_mul(r, a, b);
}

void _avxvec4_min(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_min_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_max(vec4_t r, const vec4_t a, const vec4_t b) {
    _store_vec4(r, _mm_max_ps(_load_vec4(a), _load_vec4(b)));
}

void _avxvec4_clamp(vec4_t r, const vec4_t v, const vec4_t min, const vec4_t max) {
    __m128 vv = _load_vec4(v);
    _store_vec4(r, _mm_min_ps(_mm_max_ps(vv, _load_vec4(min)), _load_vec4(max)));
}

void _avxvec4_abs(vec4_t r, const vec4_t v) {
    __m128 rv = _mm_and_ps(_load_vec4(v), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    _store_vec4(r, rv);
}

void _avxvec4_sqrt(vec4_t r, const vec4_t v) {
    _store_vec4(r, _mm_sqrt_ps(_load_vec4(v)));
}

void _avxvec4_transform(vec4_t r, const vec4_t v, const mat4_t m) {
    __m128 vv = _load_vec4(v);
    __m128 mv0 = _mm_loadu_ps(&m[0]);  // First column
    __m128 mv1 = _mm_loadu_ps(&m[4]);  // Second column
    __m128 mv2 = _mm_loadu_ps(&m[8]);  // Third column
    __m128 mv3 = _mm_loadu_ps(&m[12]); // Fourth column

    __m128 res = _mm_add_ps(
        _mm_add_ps(
            _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(0, 0, 0, 0)), mv0),
                _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(1, 1, 1, 1)), mv1)),
            _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(2, 2, 2, 2)), mv2)),
        mv3);

    _store_vec4(r, res);
}

float _avxvec4_distance_squared(const vec4_t a, const vec4_t b) {
    __m128 diff = _mm_sub_ps(_load_vec4(a), _load_vec4(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0xFF);
    return _mm_cvtss_f32(rv);
}

void _avxvec4_rotate(vec4_t r, const vec4_t v, const vec4_t axis, float angle) {
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);
    float dot_va = _avxvec4_dot(v, axis);
    vec4_t temp;
    _avxvec4_scalar_mul(temp, axis, dot_va * (1 - cos_theta));
    __m128 temp_v = _load_vec4(temp);
    __m128 v_v = _load_vec4(v);
    __m128 axis_v = _load_vec4(axis);

    __m128 axis_yzx = _mm_shuffle_ps(axis_v, axis_v, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 axis_zxy = _mm_shuffle_ps(axis_v, axis_v, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 v_yzx = _mm_shuffle_ps(v_v, v_v, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 v_zxy = _mm_shuffle_ps(v_v, v_v, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 cross_product = _mm_sub_ps(_mm_mul_ps(axis_yzx, v_zxy), _mm_mul_ps(axis_zxy, v_yzx));

    __m128 rotated_v = _mm_add_ps(
        _mm_add_ps(_mm_mul_ps(v_v, _mm_set1_ps(cos_theta)), _mm_mul_ps(cross_product, _mm_set1_ps(sin_theta))),
        temp_v
    );

    _store_vec4(r, rotated_v);
}
