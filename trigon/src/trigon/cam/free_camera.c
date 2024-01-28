#include "camera.h"
#include "core/input.h"
#include "core/core.h"
#include "shaders/shader_global.h"

static void ctrl_run() {
	camera_t* camera = camera_get_current();
	if (!camera) return;

	float dt = (float)time_dt();

	if (input_mouse_clicked(1)) {
		input_cursor_lock(true);


		float px = input_mouse_prev_x();
		float py = input_mouse_prev_y();

		float cx = input_mouse_x();
		float cy = input_mouse_y();

		float sensitivity = 70.0f;
		float x = cx - px;
		float y = py - cy;

		camera_rotate_fps(camera, (vec2) { y* sensitivity* dt, x* sensitivity* dt });
	}
	else {
		input_cursor_lock(false);
	}

	float movement_sens = 2;
	if (input_get_key(KEYCODE_LEFT_SHIFT)) {
		movement_sens = 8;
	}

	if (input_get_key(KEYCODE_W)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera->forward, movement_sens * dt, pos_to_add);
		glm_vec3_add(camera->position, pos_to_add, camera->position);
		camera_set_position(camera, camera->position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
	if (input_get_key(KEYCODE_S)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera->forward, -movement_sens * dt, pos_to_add);
		glm_vec3_add(camera->position, pos_to_add, camera->position);
		camera_set_position(camera, camera->position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}

	if (input_get_key(KEYCODE_D)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera->right, movement_sens * dt, pos_to_add);
		glm_vec3_add(camera->position, pos_to_add, camera->position);
		camera_set_position(camera, camera->position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
	if (input_get_key(KEYCODE_A)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera->right, -movement_sens * dt, pos_to_add);
		glm_vec3_add(camera->position, pos_to_add, camera->position);
		camera_set_position(camera, camera->position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
}


void free_cam_udt() {
	ctrl_run();

	camera_t* cam = camera_get_current();
	shader_global_data_t data = { 0 };
	shader_global_data_get(&data);
	glm_mat4_copy(cam->matrices.transform, data.camera[0]);
	glm_mat4_copy(cam->matrices.perspective, data.camera[1]);
	shader_global_data_set(&data);
}