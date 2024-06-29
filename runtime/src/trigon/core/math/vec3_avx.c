#include "tgmath.h"
#include <math.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

static inline __m128 _load_vec3(const vec3_t v) {
    return _mm_set_ps(0.0f, v[2], v[1], v[0]);
}

static inline void _store_vec3(vec3_t r, __m128 v) {
    _mm_store_ss(&r[0], v);
    _mm_store_ss(&r[1], _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
    _mm_store_ss(&r[2], _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)));
}

void _avxvec3_add(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_add_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_sub(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_sub_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_mul(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_mul_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_div(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_div_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_neg(vec3_t r, const vec3_t v) {
    _store_vec3(r, _mm_sub_ps(_mm_setzero_ps(), _load_vec3(v)));
}

float _avxvec3_dot(const vec3_t a, const vec3_t b) {
    __m128 rv = _mm_dp_ps(_load_vec3(a), _load_vec3(b), 0x7F);
    return _mm_cvtss_f32(rv);
}

float _avxvec3_magnitude(const vec3_t v) {
    __m128 rv = _mm_dp_ps(_load_vec3(v), _load_vec3(v), 0x7F);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec3_normalize(vec3_t r, const vec3_t v) {
    __m128 vv = _load_vec3(v);
    __m128 mv = _mm_set1_ps(_avxvec3_magnitude(v));
    _store_vec3(r, _mm_div_ps(vv, mv));
}

void _avxvec3_cross(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 av_yzx = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 av_zxy = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 bv_yzx = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 bv_zxy = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 1, 0, 2));
    _store_vec3(r, _mm_sub_ps(_mm_mul_ps(av_yzx, bv_zxy), _mm_mul_ps(av_zxy, bv_yzx)));
}

void _avxvec3_project(vec3_t r, const vec3_t a, const vec3_t b) {
    float scalar = _avxvec3_dot(a, b) / _avxvec3_dot(b, b);
    _store_vec3(r, _mm_mul_ps(_load_vec3(b), _mm_set1_ps(scalar)));
}

float _avxvec3_distance(const vec3_t a, const vec3_t b) {
    __m128 diff = _mm_sub_ps(_load_vec3(a), _load_vec3(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0x7F);
    return sqrtf(_mm_cvtss_f32(rv));
}

void _avxvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 tv = _mm_set1_ps(t);
    __m128 one_minus = _mm_sub_ps(_mm_set1_ps(1.0f), tv);
    _store_vec3(r, _mm_add_ps(_mm_mul_ps(one_minus, av), _mm_mul_ps(tv, bv)));
}

void _avxvec3_copy(vec3_t r, const vec3_t v) {
    _store_vec3(r, _load_vec3(v));
}

bool _avxvec3_eq(const vec3_t a, const vec3_t b) {
    __m128 cmp = _mm_cmpeq_ps(_load_vec3(a), _load_vec3(b));
    return (_mm_movemask_ps(cmp) & 7) == 7;
}

void _avxvec3_scalar_mul(vec3_t r, const vec3_t v, float s) {
    _store_vec3(r, _mm_mul_ps(_load_vec3(v), _mm_set1_ps(s)));
}

void _avxvec3_scalar_div(vec3_t r, const vec3_t v, float s) {
    _store_vec3(r, _mm_div_ps(_load_vec3(v), _mm_set1_ps(s)));
}

void _avxvec3_reflect(vec3_t r, const vec3_t v, const vec3_t normal) {
    __m128 nv = _load_vec3(normal);
    __m128 scalar = _mm_set1_ps(2.0f * _avxvec3_dot(v, normal));
    __m128 rv = _mm_sub_ps(_load_vec3(v), _mm_mul_ps(nv, scalar));
    _store_vec3(r, rv);
}

float _avxvec3_angle_between(const vec3_t a, const vec3_t b) {
    return acosf(_avxvec3_dot(a, b) / (_avxvec3_magnitude(a) * _avxvec3_magnitude(b)));
}

void _avxvec3_hadamard(vec3_t r, const vec3_t a, const vec3_t b) {
    _avxvec3_mul(r, a, b);
}

void _avxvec3_min(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_min_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_max(vec3_t r, const vec3_t a, const vec3_t b) {
    _store_vec3(r, _mm_max_ps(_load_vec3(a), _load_vec3(b)));
}

void _avxvec3_clamp(vec3_t r, const vec3_t v, const vec3_t min, const vec3_t max) {
    __m128 vv = _load_vec3(v);
    _store_vec3(r, _mm_min_ps(_mm_max_ps(vv, _load_vec3(min)), _load_vec3(max)));
}

void _avxvec3_abs(vec3_t r, const vec3_t v) {
    __m128 rv = _mm_and_ps(_load_vec3(v), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    _store_vec3(r, rv);
}

void _avxvec3_sqrt(vec3_t r, const vec3_t v) {
    _store_vec3(r, _mm_sqrt_ps(_load_vec3(v)));
}

void _avxvec3_transform(vec3_t r, const vec3_t v, const mat4_t m) {
    __m128 vv = _load_vec3(v);
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

    _store_vec3(r, res);
}

float _avxvec3_distance_squared(const vec3_t a, const vec3_t b) {
    __m128 diff = _mm_sub_ps(_load_vec3(a), _load_vec3(b));
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 rv = _mm_dp_ps(sq_diff, _mm_set1_ps(1.0f), 0x7F);
    return _mm_cvtss_f32(rv);
}

void _avxvec3_rotate(vec3_t r, const vec3_t v, const vec3_t axis, float angle) {
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);
    float dot_va = _avxvec3_dot(v, axis);
    vec3_t temp;
    _avxvec3_scalar_mul(temp, axis, dot_va * (1 - cos_theta));
    __m128 temp_v = _load_vec3(temp);
    __m128 v_v = _load_vec3(v);
    __m128 axis_v = _load_vec3(axis);

    __m128 axis_yzx = _mm_shuffle_ps(axis_v, axis_v, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 axis_zxy = _mm_shuffle_ps(axis_v, axis_v, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 v_yzx = _mm_shuffle_ps(v_v, v_v, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 v_zxy = _mm_shuffle_ps(v_v, v_v, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 cross_product = _mm_sub_ps(_mm_mul_ps(axis_yzx, v_zxy), _mm_mul_ps(axis_zxy, v_yzx));

    __m128 rotated_v = _mm_add_ps(
        _mm_add_ps(_mm_mul_ps(v_v, _mm_set1_ps(cos_theta)), _mm_mul_ps(cross_product, _mm_set1_ps(sin_theta))),
        temp_v
    );

    _store_vec3(r, rotated_v);
}
