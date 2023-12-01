#include "examples.h"
#include "dev/io/io.h"
#include "assets/shaders/shader.h"
#include "string.h"
#include <inttypes.h>
#include "assets/meshes/mesh.h"
#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include "entities/entity.h"
#include "dev/input/input.h"
#include "dev/time/time.h"

static shader_resource_group_t group = { 0 };
static shader_t shader;
static uint32_t mesh;
static mat4		model[4] = { 0 };
static camera_t camera = { 0 };

void controller_update() {
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

		camera_rotate_fps(&camera, (vec2) { y* sensitivity* dt, x* sensitivity* dt });
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
		glm_vec3_scale(camera.forward, movement_sens * dt, pos_to_add);
		glm_vec3_add(camera.position, pos_to_add, camera.position);
		camera_set_position(&camera, camera.position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
	if (input_get_key(KEYCODE_S)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera.forward, -movement_sens * dt, pos_to_add);
		glm_vec3_add(camera.position, pos_to_add, camera.position);
		camera_set_position(&camera, camera.position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}

	if (input_get_key(KEYCODE_D)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera.right, movement_sens * dt, pos_to_add);
		glm_vec3_add(camera.position, pos_to_add, camera.position);
		camera_set_position(&camera, camera.position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
	if (input_get_key(KEYCODE_A)) {
		vec3 pos_to_add;
		glm_vec3_scale(camera.right, -movement_sens * dt, pos_to_add);
		glm_vec3_add(camera.position, pos_to_add, camera.position);
		camera_set_position(&camera, camera.position);
		//printf("x: %f, y:%f, z:%f\n", camera->position[0], camera->position[1], camera->position[2]);
	}
}


void start_examples() {
	camera_info_t camera_info;
	camera_info.far = 1000.0f;
	camera_info.near = 0.10f;
	camera_info.fov = 45.0f;
	camera_new(&camera, &camera_info);

	shader_resource_t camera_resource = { 0 };
	shader_resource_t matrix_resource = { 0 };


	shader_resource_info_t camera_res = { 0 };
	camera_res.binding = 0;
	camera_res.count = 1;
	camera_res.type = SHADER_STATIC_BUFFER;
	camera_res.data_stride = sizeof(camera_data_t);
	camera_res.data_count = 1;
	shader_resource_new(&camera_res, &camera_resource);

	shader_resource_info_t matrix_info = { 0 };
	matrix_info.binding = 1;
	matrix_info.count = 1;
	matrix_info.type = SHADER_STATIC_BUFFER;
	matrix_info.data_stride = sizeof(mat4);
	matrix_info.data_count = 4;
	shader_resource_new(&matrix_info, &matrix_resource);

	shader_resource_group_info_t group_info = { 0 };
	group_info.resource_count = 2;

	memcpy(&group_info.resources[0], &camera_resource, sizeof(shader_resource_t));
	memcpy(&group_info.resources[1], &matrix_resource, sizeof(shader_resource_t));
	shader_resource_group_new(&group_info, &group);

	shader_info_t info = { 0 };
	info.shader_name = "vcolor";
	info.group_count = 1;
	info.groups[0] = (shader_resource_group_t*){
		&group
	};

	shader_new(&info, &shader);

	mesh_info_t mesh_info = { 0 };
	primitive_get(PRIMITIVE_CUBE, &mesh_info);
	mesh = mesh_new(&mesh_info);

	glm_mat4_identity(model[0]);
	glm_translate(model[0], (vec3) { 0, 0, 5 });

	glm_mat4_identity(model[1]);
	glm_translate(model[1], (vec3) { 0, 0, -5 });

	glm_mat4_identity(model[2]);
	glm_translate(model[2], (vec3) { 5, 0, 0 });

	glm_mat4_identity(model[3]);
	glm_translate(model[3], (vec3) { -5, 0, 0 });

	//glm_lookat((vec3) { 10, 2, 10 }, (vec3) { 0, 0, 0 }, (vec3) { 0, 1, 0 }, camera.data.transform);
	controller_update();
}


void run_examples() {
	controller_update();

	shader_resource_set(&shader.groups[0]->resources[0], &camera.data);
	shader_resource_set(&shader.groups[0]->resources[1], &model);
	shader_use(&shader);

	mesh_bind(mesh);
	mesh_draw(mesh, 4);


}

void stop_examples() {
	mesh_clear();
	shader_resource_group_del(&group);
	shader_del(&shader);
}