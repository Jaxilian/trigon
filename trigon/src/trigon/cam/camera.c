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

    glm_lookat(camera->position, look_at, up, camera->matrices.transform);

    glm_vec3_copy(right, camera->right);
    glm_vec3_copy(forward, camera->forward);
    glm_vec3_copy(up, camera->up);
}

void camera_set_position(camera_t* camera, vec3 position) {
    glm_vec3_copy(position, camera->position);
    camera->matrices.transform[3][0] = position[0];
    camera->matrices.transform[3][1] = position[1];
    camera->matrices.transform[3][2] = position[2];

    vec3 look_at;
    glm_vec3_add(camera->position, camera->forward, look_at);
    glm_lookat(camera->position, look_at, camera->up, camera->matrices.transform);
}

/*
void screenPosToWorldRay(
    int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
    int screenWidth, int screenHeight,  // Window size, in pixels
    mat4 viewMatrix,                    // Camera view matrix
    mat4 projectionMatrix,              // Camera projection matrix
    vec3 out_origin,                    // Ray origin (output)
    vec3 out_direction                  // Ray direction (output)
) {
    // Convert screen position to NDC
    vec4 rayStartNDC = {
        (float)mouseX / (float)screenWidth  - 0.5f,
        (float)mouseY / (float)screenHeight - 0.5f,
        -1.0, // Near plane
        1.0f
    };
    vec4 rayEndNDC = {
        rayStartNDC.x,
        rayStartNDC.y,
        0.0, // Far plane
        1.0f
    };

    // Convert NDC to clip space
    rayStartNDC.x *= 2.0f;
    rayStartNDC.y *= 2.0f;
    rayEndNDC.x *= 2.0f;
    rayEndNDC.y *= 2.0f;

    // Convert to world space
    mat4 invProjMat, invViewMat;
    glm_mat4_inv(projectionMatrix, invProjMat);
    glm_mat4_inv(viewMatrix, invViewMat);

    vec4 rayStartWorld, rayEndWorld;
    glm_mat4_mulv(invProjMat, rayStartNDC, rayStartWorld);
    glm_mat4_mulv(invProjMat, rayEndNDC, rayEndWorld);

    // Convert from homogenous to 3D coordinates
    glm_vec4_divs(rayStartWorld, rayStartWorld[3], rayStartWorld);
    glm_vec4_divs(rayEndWorld, rayEndWorld[3], rayEndWorld);

    // Determine ray origin and direction
    glm_vec4_copy(rayStartWorld, out_origin);
    glm_vec3_sub(rayEndWorld, rayStartWorld, out_direction);
    glm_vec3_normalize(out_direction);
}

// Usage example
int mouseX, mouseY;
int screenWidth, screenHeight;
mat4 viewMatrix, projectionMatrix;
vec3 rayOrigin, rayDirection;

// Initialize viewMatrix, projectionMatrix, mouseX, mouseY, screenWidth, screenHeight
screenPosToWorldRay(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projectionMatrix, rayOrigin, rayDirection);

// Perform raycasting with rayOrigin and rayDirection*/