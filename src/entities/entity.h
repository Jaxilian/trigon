#ifndef ENTITY_H
#define ENTITY_H
#include <cglm/cglm.h>

typedef mat4 transform_t;

typedef struct {
	float   fov;
	float   near;
	float   far;
} camera_info_t;

typedef struct {
	transform_t		transform;
	mat4			perspective;
} camera_data_t;

typedef struct {
	camera_data_t	data;
	camera_info_t	info;

	vec3    position;
	vec3    forward;
	vec3    up;
	vec3    right;
	float   yaw;
	float   pitch;
} camera_t;

void camera_new(camera_t* camera, camera_info_t* info);
void camera_rotate_fps(camera_t* camera, vec2 eular);
void camera_set_position(camera_t* camera, vec3 position);

#endif // !ENTITY_H
