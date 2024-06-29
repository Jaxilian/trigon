#ifndef TG_MESH_H
#define TG_MESH_H
#include "trigon/math/tgmath.h"

typedef struct {
	vec3_t position;
	vec3_t normal;
	vec3_t color;
	vec2_t uv;
} vertex3_t;

typedef struct {
	vec3_t position;
	vec2_t uv;
} vertex2_t;

#endif