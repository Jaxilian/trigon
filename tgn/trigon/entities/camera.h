#ifndef TGN_CAMERA_H
#define TGN_CAMERA_H

#include "entity.h"
#include "gfx/core/gfxctx.h"

typedef struct {
	entity_t	base;
	mat4		projection;
	float		fov, near, far;
} camera_t;

typedef struct {
	float	fov, near, far;
	vec3	look_at;
	vec3	position;
	win_t	window;
} camera_info_t;

void camera_new(camera_t* cam, camera_info_t* info);
void camera_del(camera_t* cam);

#endif