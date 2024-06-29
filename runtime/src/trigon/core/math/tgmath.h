#ifndef TG_MATH_H
#define TG_MATH_H

#include <stdbool.h>

typedef float vec4_t[4];
typedef float vec3_t[3];
typedef float vec2_t[2];
typedef float mat4_t[16];

void _tgmath_init(); // super important to call before math

bool _avx_supported();

#endif // !TG_MATH_H
