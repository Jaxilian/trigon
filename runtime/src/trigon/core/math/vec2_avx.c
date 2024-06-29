#include "tgmath.h"
#include <math.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

static inline __m128 _load_vec2(const vec2_t v) {
    return _mm_set_ps(0.0f, 0.0f, v[1], v[0]);
}

static inline void _store_vec2(vec2_t r, __m128 v) {
    _mm_store_ss(&r[0], v);
    _mm_store_ss(&r[1], _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
}

void _avxvec2_add(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_add_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_sub(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_sub_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_mul(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_mul_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_div(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_div_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_neg(vec2_t r, const vec2_t v) {
    _store_vec2(r, _mm_sub_ps(_mm_setzero_ps(), _load_vec2(v)));
}

float _avxvec2_dot(const vec2_t a, const vec2_t b) {
    __m128 rv = _mm_dp_ps(_load_vec2(a), _load_vec2(b), 0x3F);
    return _mm_cvtss_f32(rv);
}

float _avxvec2_magnitude(const vec2_t v) {
    __m128 rv = _mm_dp_ps(_load_vec2(v), _load_vec2(v), 0x3F);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec2_normalize(vec2_t r, const vec2_t v) {
    __m128 vv = _load_vec2(v);
    __m128 mv = _mm_set1_ps(_avxvec2_magnitude(v));
    _store_vec2(r, _mm_div_ps(vv, mv));
}

void _avxvec2_project(vec2_t r, const vec2_t a, const vec2_t b) {
    float scalar = _avxvec2_dot(a, b) / _avxvec2_dot(b, b);
    _store_vec2(r, _mm_mul_ps(_load_vec2(b), _mm_set1_ps(scalar)));
}

float _avxvec2_distance(const vec2_t a, const vec2_t b) {
    __m128 diff = _mm_sub_ps(_load_vec2(a), _load_vec2(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0x3F);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec2_lerp(vec2_t r, const vec2_t a, const vec2_t b, float t) {
    __m128 av = _load_vec2(a);
    __m128 bv = _load_vec2(b);
    __m128 tv = _mm_set1_ps(t);
    __m128 one_minus = _mm_sub_ps(_mm_set1_ps(1.0f), tv);
    _store_vec2(r, _mm_add_ps(_mm_mul_ps(one_minus, av), _mm_mul_ps(tv, bv)));
}

void _avxvec2_copy(vec2_t r, const vec2_t v) {
    _store_vec2(r, _load_vec2(v));
}

bool _avxvec2_eq(const vec2_t a, const vec2_t b) {
    __m128 cmp = _mm_cmpeq_ps(_load_vec2(a), _load_vec2(b));
    return (_mm_movemask_ps(cmp) & 3) == 3;
}

void _avxvec2_scalar_mul(vec2_t r, const vec2_t v, float s) {
    _store_vec2(r, _mm_mul_ps(_load_vec2(v), _mm_set1_ps(s)));
}

void _avxvec2_scalar_div(vec2_t r, const vec2_t v, float s) {
    _store_vec2(r, _mm_div_ps(_load_vec2(v), _mm_set1_ps(s)));
}

void _avxvec2_reflect(vec2_t r, const vec2_t v, const vec2_t normal) {
    __m128 nv = _load_vec2(normal);
    __m128 scalar = _mm_set1_ps(2.0f * _avxvec2_dot(v, normal));
    __m128 rv = _mm_sub_ps(_load_vec2(v), _mm_mul_ps(nv, scalar));
    _store_vec2(r, rv);
}

float _avxvec2_angle_between(const vec2_t a, const vec2_t b) {
    return acosf(_avxvec2_dot(a, b) / (_avxvec2_magnitude(a) * _avxvec2_magnitude(b)));
}

void _avxvec2_hadamard(vec2_t r, const vec2_t a, const vec2_t b) {
    _avxvec2_mul(r, a, b);
}

void _avxvec2_min(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_min_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_max(vec2_t r, const vec2_t a, const vec2_t b) {
    _store_vec2(r, _mm_max_ps(_load_vec2(a), _load_vec2(b)));
}

void _avxvec2_clamp(vec2_t r, const vec2_t v, const vec2_t min, const vec2_t max) {
    __m128 vv = _load_vec2(v);
    _store_vec2(r, _mm_min_ps(_mm_max_ps(vv, _load_vec2(min)), _load_vec2(max)));
}

void _avxvec2_abs(vec2_t r, const vec2_t v) {
    __m128 rv = _mm_and_ps(_load_vec2(v), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    _store_vec2(r, rv);
}

void _avxvec2_sqrt(vec2_t r, const vec2_t v) {
    _store_vec2(r, _mm_sqrt_ps(_load_vec2(v)));
}

float _avxvec2_distance_squared(const vec2_t a, const vec2_t b) {
    __m128 diff = _mm_sub_ps(_load_vec2(a), _load_vec2(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0x3F);
    return _mm_cvtss_f32(rv);
}

void _avxvec2_rotate(vec2_t r, const vec2_t v, float angle) {
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);
    __m128 rot_matrix_col0 = _mm_set_ps(0.0f, 0.0f, cos_theta, -sin_theta);
    __m128 rot_matrix_col1 = _mm_set_ps(0.0f, 0.0f, sin_theta, cos_theta);
    __m128 vv = _load_vec2(v);

    __m128 result = _mm_add_ps(
        _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(2, 2, 2, 2)), rot_matrix_col0),
        _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(3, 3, 3, 3)), rot_matrix_col1)
    );

    _store_vec2(r, result);
}
