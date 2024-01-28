#include "camera.h"
#include <memory.h>
#include "core/core.h"

static camera_t* current = NULL;


void camera_new(camera_t* camera, float fov, float near, float far) {
    memset(camera, 0, sizeof(camera_t));
    camera->fov = fov;
    camera->near = near;
    camera->far = far;
    glm_mat4_identity(camera->matrices.transform);

    ivec2 extent = { 0 };
    trigon_core_win_extent(extent);

    glm_perspective(
        camera->fov,
        (float)extent[0]/ (float)extent[1],
        0.1f,
        1000.0f,
        camera->matrices.perspective
    );

    if (current == NULL) {
        current = camera;
    }
}

void camera_set_current(camera_t* camera) {
	current = camera;
}

camera_t* camera_get_current() {
	return current;
}