#ifndef TG_AVXMATH_H
#define TG_AVXMATH_H

#include "tgmath.h"

void _avxvec2_add(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_sub(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_mul(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_div(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_neg(vec2_t r, const vec2_t v);
float _avxvec2_dot(const vec2_t a, const vec2_t b);
float _avxvec2_magnitude(const vec2_t v);
void _avxvec2_normalize(vec2_t r, const vec2_t v);
void _avxvec2_project(vec2_t r, const vec2_t a, const vec2_t b);
float _avxvec2_distance(const vec2_t a, const vec2_t b);
void _avxvec2_lerp(vec2_t r, const vec2_t a, const vec2_t b, float t);
void _avxvec2_copy(vec2_t r, const vec2_t v);
bool _avxvec2_eq(const vec2_t a, const vec2_t b);
void _avxvec2_scalar_mul(vec2_t r, const vec2_t v, float s);
void _avxvec2_scalar_div(vec2_t r, const vec2_t v, float s);
void _avxvec2_reflect(vec2_t r, const vec2_t v, const vec2_t normal);
float _avxvec2_angle_between(const vec2_t a, const vec2_t b);
void _avxvec2_hadamard(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_min(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_max(vec2_t r, const vec2_t a, const vec2_t b);
void _avxvec2_clamp(vec2_t r, const vec2_t v, const vec2_t min, const vec2_t max);
void _avxvec2_abs(vec2_t r, const vec2_t v);
void _avxvec2_sqrt(vec2_t r, const vec2_t v);
float _avxvec2_distance_squared(const vec2_t a, const vec2_t b);
void _avxvec2_rotate(vec2_t r, const vec2_t v, float angle);

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

void _avxvec4_add(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_sub(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_mul(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_div(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_neg(vec4_t r, const vec4_t v);
float _avxvec4_dot(const vec4_t a, const vec4_t b);
float _avxvec4_magnitude(const vec4_t v);
void _avxvec4_normalize(vec4_t r, const vec4_t v);
void _avxvec4_cross(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_project(vec4_t r, const vec4_t a, const vec4_t b);
float _avxvec4_distance(const vec4_t a, const vec4_t b);
void _avxvec4_lerp(vec4_t r, const vec4_t a, const vec4_t b, float t);
void _avxvec4_copy(vec4_t r, const vec4_t v);
bool _avxvec4_eq(const vec4_t a, const vec4_t b);
void _avxvec4_scalar_mul(vec4_t r, const vec4_t v, float s);
void _avxvec4_scalar_div(vec4_t r, const vec4_t v, float s);
void _avxvec4_reflect(vec4_t r, const vec4_t v, const vec4_t normal);
float _avxvec4_angle_between(const vec4_t a, const vec4_t b);
void _avxvec4_hadamard(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_min(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_max(vec4_t r, const vec4_t a, const vec4_t b);
void _avxvec4_clamp(vec4_t r, const vec4_t v, const vec4_t min, const vec4_t max);
void _avxvec4_abs(vec4_t r, const vec4_t v);
void _avxvec4_sqrt(vec4_t r, const vec4_t v);
void _avxvec4_transform(vec4_t r, const vec4_t v, const mat4_t m);
float _avxvec4_distance_squared(const vec4_t a, const vec4_t b);
void _avxvec4_rotate(vec4_t r, const vec4_t v, const vec4_t axis, float angle);

void _avxmat4_mul(mat4_t r, const mat4_t a, const mat4_t b);
void _avxmat4_setpos(mat4_t r, const vec3_t t);
void _avxmat4_translate(mat4_t r, const vec3_t t);



#endif // !AVXMATH_H

