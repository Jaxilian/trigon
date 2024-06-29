#ifndef TG_MATH_H
#define TG_MATH_H

#include <stdbool.h>

#define mat4_identity {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f}

typedef float vec4_t[4];
typedef float vec3_t[3];
typedef float vec2_t[2];
typedef float mat4_t[16];

typedef void (*mat4_mul_ptr)(mat4_t r, const mat4_t a, const mat4_t b);

extern mat4_mul_ptr mat4_mul;

// AVX

void  _avxvec3_add(vec3_t r, const vec3_t a, const vec3_t b);
void  _avxvec3_sub(vec3_t r, const vec3_t a, const vec3_t b);
void  _avxvec3_mul(vec3_t r, const vec3_t a, const vec3_t b);
void  _avxvec3_div(vec3_t r, const vec3_t a, const vec3_t b);
void  _avxvec3_neg(vec3_t r, const vec3_t v);
float _avxvec3_dot(const vec3_t a, const vec3_t b);
float _avxvec3_magnitude(const vec3_t v);
void  _avxvec3_normalize(vec3_t r, const vec3_t v);
void  _avxvec3_cross(vec3_t r, const vec3_t a, const vec3_t b);
void  _avxvec3_project(vec3_t r, const vec3_t a, const vec3_t b);
float _avxvec3_distance(const vec3_t a, const vec3_t b);
void  _avxvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t);

void _avxvec2_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _avxvec4_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _avxmat4_mul(mat4_t r, const mat4_t a, const mat4_t b);

// SSE
void _ssemat4_mul(mat4_t r, const mat4_t a, const mat4_t b);

void _tgmath_init(); // super important to call before math
bool _avx_supported();


#endif // TG_MATH_H
