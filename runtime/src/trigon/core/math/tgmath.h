#ifndef TG_MATH_H
#define TG_MATH_H

#include <stdbool.h>

#define PI 3.14159265358979323846

#define mat4_identity {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f}

typedef float vec4_t[4];
typedef float vec3_t[3];
typedef float vec2_t[2];
typedef float mat4_t[16];

typedef void (*mat4_mul_ptr)(mat4_t r, const mat4_t a, const mat4_t b);

extern mat4_mul_ptr mat4_mul;

// AVX

void _avxvec3_add(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_sub(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_div(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_neg(vec3_t r, const vec3_t v);
float _avxvec3_dot(const vec3_t a, const vec3_t b);
float _avxvec3_magnitude(const vec3_t v);
void _avxvec3_normalize(vec3_t r, const vec3_t v);
void _avxvec3_cross(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_project(vec3_t r, const vec3_t a, const vec3_t b);
float _avxvec3_distance(const vec3_t a, const vec3_t b);
void _avxvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t);
void _avxvec3_copy(vec3_t r, const vec3_t v);
bool _avxvec3_eq(const vec3_t a, const vec3_t b);
void _avxvec3_scalar_mul(vec3_t r, const vec3_t v, float s);
void _avxvec3_scalar_div(vec3_t r, const vec3_t v, float s);
void _avxvec3_reflect(vec3_t r, const vec3_t v, const vec3_t normal);
float _avxvec3_angle_between(const vec3_t a, const vec3_t b);
void _avxvec3_hadamard(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_min(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_max(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec3_clamp(vec3_t r, const vec3_t v, const vec3_t min, const vec3_t max);
void _avxvec3_abs(vec3_t r, const vec3_t v);
void _avxvec3_sqrt(vec3_t r, const vec3_t v);
void _avxvec3_transform(vec3_t r, const vec3_t v, const mat4_t m);
float _avxvec3_distance_squared(const vec3_t a, const vec3_t b);
void _avxvec3_rotate(vec3_t r, const vec3_t v, const vec3_t axis, float angle);

void _avxvec2_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec4_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _avxmat4_mul(mat4_t r, const mat4_t a, const mat4_t b);

// SSE
void _ssemat4_mul(mat4_t r, const mat4_t a, const mat4_t b);

void _tgmath_init(); // super important to call before math
bool _avx_supported();


#endif // TG_MATH_H
