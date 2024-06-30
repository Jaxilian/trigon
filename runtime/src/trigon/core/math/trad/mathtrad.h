#ifndef TG_MATHTRAD_H
#define TG_MATHTRAD_H
#include "../mathr.h"
#include <stdbool.h>

void _tradvec4_add(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_sub(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_mul(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_div(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_neg(vec4_t r, const vec4_t v);
float _tradvec4_dot(const vec4_t a, const vec4_t b);
float _tradvec4_magnitude(const vec4_t v);
void _tradvec4_normalize(vec4_t r, const vec4_t v);
void _tradvec4_cross(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_project(vec4_t r, const vec4_t a, const vec4_t b);
float _tradvec4_distance(const vec4_t a, const vec4_t b);
void _tradvec4_lerp(vec4_t r, const vec4_t a, const vec4_t b, float t);
void _tradvec4_copy(vec4_t r, const vec4_t v);
bool _tradvec4_eq(const vec4_t a, const vec4_t b);
void _tradvec4_scalar_mul(vec4_t r, const vec4_t v, float s);
void _tradvec4_scalar_div(vec4_t r, const vec4_t v, float s);
void _tradvec4_reflect(vec4_t r, const vec4_t v, const vec4_t normal);
float _tradvec4_angle_between(const vec4_t a, const vec4_t b);
void _tradvec4_hadamard(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_min(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_max(vec4_t r, const vec4_t a, const vec4_t b);
void _tradvec4_clamp(vec4_t r, const vec4_t v, const vec4_t min, const vec4_t max);
void _tradvec4_abs(vec4_t r, const vec4_t v);
void _tradvec4_sqrt(vec4_t r, const vec4_t v);
float _tradvec4_distance_squared(const vec4_t a, const vec4_t b);
void _tradvec4_rotate(vec4_t r, const vec4_t v, const vec4_t axis, float angle);
void _tradvec4_test();

void _tradvec3_add(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_sub(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_mul(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_div(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_neg(vec3_t r, const vec3_t v);
float _tradvec3_dot(const vec3_t a, const vec3_t b);
float _tradvec3_magnitude(const vec3_t v);
void _tradvec3_normalize(vec3_t r, const vec3_t v);
void _tradvec3_cross(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_project(vec3_t r, const vec3_t a, const vec3_t b);
float _tradvec3_distance(const vec3_t a, const vec3_t b);
void _tradvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t);
void _tradvec3_copy(vec3_t r, const vec3_t v);
bool _tradvec3_eq(const vec3_t a, const vec3_t b);
void _tradvec3_scalar_mul(vec3_t r, const vec3_t v, float s);
void _tradvec3_scalar_div(vec3_t r, const vec3_t v, float s);
void _tradvec3_reflect(vec3_t r, const vec3_t v, const vec3_t normal);
float _tradvec3_angle_between(const vec3_t a, const vec3_t b);
void _tradvec3_hadamard(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_min(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_max(vec3_t r, const vec3_t a, const vec3_t b);
void _tradvec3_clamp(vec3_t r, const vec3_t v, const vec3_t min, const vec3_t max);
void _tradvec3_abs(vec3_t r, const vec3_t v);
void _tradvec3_sqrt(vec3_t r, const vec3_t v);
float _tradvec3_distance_squared(const vec3_t a, const vec3_t b);
void _tradvec3_rotate(vec3_t r, const vec3_t v, const vec3_t axis, float angle);
void _tradvec3_test();

void _tradvec2_add(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_sub(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_mul(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_div(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_neg(vec2_t r, const vec2_t v);
float _tradvec2_dot(const vec2_t a, const vec2_t b);
float _tradvec2_magnitude(const vec2_t v);
void _tradvec2_normalize(vec2_t r, const vec2_t v);
void _tradvec2_cross(vec2_t r, const vec2_t a, const vec2_t b); // Cross product for 2D returns a scalar
void _tradvec2_project(vec2_t r, const vec2_t a, const vec2_t b);
float _tradvec2_distance(const vec2_t a, const vec2_t b);
void _tradvec2_lerp(vec2_t r, const vec2_t a, const vec2_t b, float t);
void _tradvec2_copy(vec2_t r, const vec2_t v);
bool _tradvec2_eq(const vec2_t a, const vec2_t b);
void _tradvec2_scalar_mul(vec2_t r, const vec2_t v, float s);
void _tradvec2_scalar_div(vec2_t r, const vec2_t v, float s);
void _tradvec2_reflect(vec2_t r, const vec2_t v, const vec2_t normal);
float _tradvec2_angle_between(const vec2_t a, const vec2_t b);
void _tradvec2_hadamard(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_min(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_max(vec2_t r, const vec2_t a, const vec2_t b);
void _tradvec2_clamp(vec2_t r, const vec2_t v, const vec2_t min, const vec2_t max);
void _tradvec2_abs(vec2_t r, const vec2_t v);
void _tradvec2_sqrt(vec2_t r, const vec2_t v);
float _tradvec2_distance_squared(const vec2_t a, const vec2_t b);
void _tradvec2_test();

void _tradmat4_mul(mat4_t r, const mat4_t a, const mat4_t b);
void _tradmat4_setscale(mat4_t r, const vec3_t s);
void _tradmat4_setpos(mat4_t r, const vec3_t s);
void _tradmat4_scale(mat4_t r, const vec3_t s);
void _tradmat4_lookat(mat4_t r, const vec3_t eye, const vec3_t center, const vec3_t up);
void _tradmat4_perspective(mat4_t r, float fovy, float aspect, float znear, float zfar);
void _tradmat4_ortho(mat4_t r, float left, float right, float bottom, float top, float znear, float zfar);
void _tradmat4_translate(mat4_t r, const vec3_t t);
void _tradmat4_rotate(mat4_t r, float angle, const vec3_t axis);
void _tradmat4_transpose(mat4_t r, const mat4_t m);
bool _tradmat4_inverse(mat4_t r, const mat4_t m);
void _tradmat4_from_quat(mat4_t r, const quat_t q);
void _tradmat4_to_quat(quat_t q, const mat4_t m);
float _tradmat4_determinant(const mat4_t m);
bool _tradmat4_inverse_affine(mat4_t r, const mat4_t m);
void _tradmat4_forward(vec3_t forward, const mat4_t m);
void _tradmat4_up(vec3_t up, const mat4_t m);
void _tradmat4_right(vec3_t right, const mat4_t m);
void _tradmat4_position(vec3_t position, const mat4_t m);
void _tradmat4_test();



#endif // !TG_MATHTRAD_H
