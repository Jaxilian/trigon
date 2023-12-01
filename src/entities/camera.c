#include "entity.h"
#include <memory.h>
#include "dev/mem/core_ctx_def.h"
#include "dev/win/win.h"

static camera_t* current = NULL;
static int32_t win_resize_callback_id = -1;

static void win_resize_cb() {
    if (!current) return;
    glm_perspective(
        current->info.fov,
        (float)ctx->win.width / (float)ctx->win.height,
        current->info.near,
        current->info.far,
        current->data.perspective
    );
}

void camera_new(camera_t* camera, camera_info_t* info) {
    memset(camera, 0, sizeof(camera_t));
    memcpy(&camera->info, info, sizeof(camera_info_t));
    glm_mat4_identity(camera->data.transform);

    glm_perspective(
        info->fov,
        (float)ctx->win.width / (float)ctx->win.height,
        0.1f,
        1000.0f,
        camera->data.perspective
    );

    if (current == NULL) {
        current = camera;

        if (win_resize_callback_id == -1) {
            win_resize_callback_id = win_window_resize_connect(win_resize_cb);
        }
    }
}

void camera_rotate_fps(camera_t* camera, vec2 eular) {
    camera->yaw += eular[1];
    camera->pitch -= eular[0];

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3 forward;
    forward[0] = (float)cos(glm_rad(camera->yaw)) * (float)cos(glm_rad(camera->pitch));
    forward[1] = (float)sin(glm_rad(camera->pitch));
    forward[2] = (float)sin(glm_rad(camera->yaw)) * (float)cos(glm_rad(camera->pitch));
    glm_normalize(forward);

    vec3 right, up = { 0,1,0 };
    glm_vec3_cross(forward, up, right);
    glm_normalize(right);

    glm_cross(right, forward, up);
    glm_normalize(up);

    vec3 look_at;
    glm_vec3_add(camera->position, forward, look_at);

    glm_lookat(camera->position, look_at, up, camera->data.transform);

    glm_vec3_copy(right, camera->right);
    glm_vec3_copy(forward, camera->forward);
    glm_vec3_copy(up, camera->up);
}

void camera_set_position(camera_t* camera, vec3 position) {
    glm_vec3_copy(position, camera->position);
    camera->data.transform[3][0] = position[0];
    camera->data.transform[3][1] = position[1];
    camera->data.transform[3][2] = position[2];

    vec3 look_at;
    glm_vec3_add(camera->position, camera->forward, look_at);
    glm_lookat(camera->position, look_at, camera->up, camera->data.transform);
}

