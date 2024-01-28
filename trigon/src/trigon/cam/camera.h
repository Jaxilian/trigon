#ifndef TRIGON_CAMERA_H
#define TRIGON_CAMERA_H

#include <cglm/cglm.h>
typedef struct {
	mat4	transform;
	mat4	perspective;
} camera_matrices_t;

typedef struct {
	camera_matrices_t	matrices;

	float   fov;
	float   near;
	float   far;
	vec3    position;
	vec3    forward;
	vec3    up;
	vec3    right;
	float   yaw;
	float   pitch;

} camera_t;

void camera_new(camera_t* camera, float fov, float near, float far);
void camera_rotate_fps(camera_t* camera, vec2 eular);
void camera_set_position(camera_t* camera, vec3 position);
void camera_set_current(camera_t* camera);
camera_t* camera_get_current();
void free_cam_udt();

#endif // !TRIGON_CAMERA_H
