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

void _avxmath_test();

// SSE
void _ssemat4_mul(mat4_t r, const mat4_t a, const mat4_t b);

void _tgmath_init(); // super important to call before math
bool _avx_supported();

void mat4_print(const char* label, const mat4_t m);


#endif // TG_MATH_H
