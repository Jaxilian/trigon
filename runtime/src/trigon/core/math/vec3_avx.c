#include "tgmath.h"

#include <math.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>


static inline __m128 _load_vec3(const vec3_t v) {
    return _mm_set_ps(0.0f, v[2], v[1], v[0]);
}

static inline void _store_vec3(vec3_t r, __m128 v) {
    r[0] = _mm_cvtss_f32(v);
    r[1] = _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
    r[2] = _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)));
}

void _avxvec3_add(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_add_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_sub(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_sub_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_mul(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_mul_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_div(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_div_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_neg(vec3_t r, const vec3_t v) {
    __m128 vv = _load_vec3(v);
    __m128 zero = _mm_setzero_ps();
    __m128 rv = _mm_sub_ps(zero, vv);
    _store_vec3(r, rv);
}

float _avxvec3_dot(const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_mul_ps(av, bv);
    __m128 shuf = _mm_movehdup_ps(rv);
    __m128 sums = _mm_add_ps(rv, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    float result;
    _mm_store_ss(&result, sums);
    return result;
}

float _avxvec3_magnitude(const vec3_t v) {
    __m128 vv = _load_vec3(v);
    __m128 rv = _mm_mul_ps(vv, vv);
    __m128 shuf = _mm_movehdup_ps(rv);
    __m128 sums = _mm_add_ps(rv, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    float result;
    _mm_store_ss(&result, sums);
    return sqrtf(result);
}

void _avxvec3_normalize(vec3_t r, const vec3_t v) {
    float magnitude = _avxvec3_magnitude(v);
    __m128 vv = _load_vec3(v);
    __m128 mv = _mm_set1_ps(magnitude);
    __m128 rv = _mm_div_ps(vv, mv);
    _store_vec3(r, rv);
}

void _avxvec3_cross(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);

    __m128 av_yzx = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 av_zxy = _mm_shuffle_ps(av, av, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 bv_yzx = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 bv_zxy = _mm_shuffle_ps(bv, bv, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 result = _mm_sub_ps(
        _mm_mul_ps(av_yzx, bv_zxy),
        _mm_mul_ps(av_zxy, bv_yzx)
    );

    _store_vec3(r, result);
}

void _avxvec3_project(vec3_t r, const vec3_t a, const vec3_t b) {
    float dot_ab = _avxvec3_dot(a, b);
    float dot_bb = _avxvec3_dot(b, b);
    float scalar = dot_ab / dot_bb;
    __m128 bv = _load_vec3(b);
    __m128 sv = _mm_set1_ps(scalar);
    __m128 rv = _mm_mul_ps(bv, sv);
    _store_vec3(r, rv);
}

float _avxvec3_distance(const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 diff = _mm_sub_ps(av, bv);
    __m128 sq_diff = _mm_mul_ps(diff, diff);
    __m128 shuf = _mm_movehdup_ps(sq_diff);
    __m128 sums = _mm_add_ps(sq_diff, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    float result;
    _mm_store_ss(&result, sums);
    return sqrtf(result);
}

void _avxvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 tv = _mm_set1_ps(t);
    __m128 one_minus = _mm_sub_ps(_mm_set1_ps(1.0f), tv);
    __m128 result = _mm_add_ps(_mm_mul_ps(one_minus, av), _mm_mul_ps(tv, bv));
    _store_vec3(r, result);
}

void _avxvec3_copy(vec3_t r, const vec3_t v) {
    __m128 vv = _load_vec3(v);
    _store_vec3(r, vv);
}

bool _avxvec3_eq(const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 cmp = _mm_cmpeq_ps(av, bv);
    return (_mm_movemask_ps(cmp) & 7) == 7;
}

void _avxvec3_scalar_mul(vec3_t r, const vec3_t v, float s) {
    __m128 vv = _load_vec3(v);
    __m128 sv = _mm_set1_ps(s);
    __m128 rv = _mm_mul_ps(vv, sv);
    _store_vec3(r, rv);
}

void _avxvec3_scalar_div(vec3_t r, const vec3_t v, float s) {
    __m128 vv = _load_vec3(v);
    __m128 sv = _mm_set1_ps(s);
    __m128 rv = _mm_div_ps(vv, sv);
    _store_vec3(r, rv);
}

void _avxvec3_reflect(vec3_t r, const vec3_t v, const vec3_t normal) {
    float dot_vn = _avxvec3_dot(v, normal);
    __m128 nv = _load_vec3(normal);
    __m128 scalar = _mm_set1_ps(2.0f * dot_vn);
    __m128 rv = _mm_mul_ps(nv, scalar);
    __m128 vv = _load_vec3(v);
    rv = _mm_sub_ps(vv, rv);
    _store_vec3(r, rv);
}

float _avxvec3_angle_between(const vec3_t a, const vec3_t b) {
    float dot_ab = _avxvec3_dot(a, b);
    float mag_a = _avxvec3_magnitude(a);
    float mag_b = _avxvec3_magnitude(b);
    return acosf(dot_ab / (mag_a * mag_b));
}

void _avxvec3_hadamard(vec3_t r, const vec3_t a, const vec3_t b) {
    _avxvec3_mul(r, a, b);
}

void _avxvec3_min(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_min_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_max(vec3_t r, const vec3_t a, const vec3_t b) {
    __m128 av = _load_vec3(a);
    __m128 bv = _load_vec3(b);
    __m128 rv = _mm_max_ps(av, bv);
    _store_vec3(r, rv);
}

void _avxvec3_clamp(vec3_t r, const vec3_t v, const vec3_t min, const vec3_t max) {
    __m128 vv = _load_vec3(v);
    __m128 minv = _load_vec3(min);
    __m128 maxv = _load_vec3(max);
    __m128 rv = _mm_min_ps(_mm_max_ps(vv, minv), maxv);
    _store_vec3(r, rv);
}

void _avxvec3_abs(vec3_t r, const vec3_t v) {
    __m128 vv = _load_vec3(v);
    __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
    __m128 rv = _mm_and_ps(vv, mask);
    _store_vec3(r, rv);
}

void _avxvec3_sqrt(vec3_t r, const vec3_t v) {
    __m128 vv = _load_vec3(v);
    __m128 rv = _mm_sqrt_ps(vv);
    _store_vec3(r, rv);
}

void _avxvec3_transform(vec3_t r, const vec3_t v, const mat4_t m) {
    __m128 vv = _load_vec3(v);
    __m128 mv0 = _mm_loadu_ps(&m[0]); 
    __m128 mv1 = _mm_loadu_ps(&m[4]); 
    __m128 mv2 = _mm_loadu_ps(&m[8]); 
    __m128 mv3 = _mm_loadu_ps(&m[12]);

    __m128 res = _mm_add_ps(
        _mm_add_ps(
            _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(0, 0, 0, 0)), mv0),
            _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(1, 1, 1, 1)), mv1)),
        _mm_add_ps(
            _mm_mul_ps(_mm_shuffle_ps(vv, vv, _MM_SHUFFLE(2, 2, 2, 2)), mv2),
            mv3));

    _store_vec3(r, res);
}