#include "camera.h"


void camera_new(camera_t* cam, camera_info_t* info) {
    cam->fov    = info->fov;
    cam->near   = info->near;
    cam->far    = info->far;

    glm_perspective(
        cam->fov,
        info->window.aspect,
        cam->near,
        cam->far,
        cam->projection
    );

    glm_lookat(
        info->position,
        info->look_at,
        (vec3) {0, 1, 0 },
        cam->base.transform.matrix
    );

    glm_vec3_copy((vec3) { 0, 1, 0 }, cam->base.transform.up);

    glm_vec3_sub(info->position, info->look_at, cam->base.transform.forward);
    glm_vec3_normalize(cam->base.transform.forward);

    glm_vec3_cross(cam->base.transform.forward, cam->base.transform.up, cam->base.transform.right);
    glm_normalize(cam->base.transform.right);

    glm_cross(cam->base.transform.right, cam->base.transform.forward, cam->base.transform.up);
    glm_normalize(cam->base.transform.up);

}