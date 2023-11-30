#include "examples.h"
#include "dev/io/io.h"
#include "assets/shaders/shader.h"
#include "string.h"
#include <inttypes.h>
#include "assets/meshes/mesh.h"
#include <cglm/cglm.h>
#include <cglm/mat4.h>

static shader_resource_group_t group = { 0 };
static shader_t shader;
static uint32_t mesh;
static mat4 model[4] = { 0 };

typedef struct {
	mat4 camera_view;
	mat4 camera_projection;
	vec3 light_direction;
} global_test_data_t;
global_test_data_t camera;

void start_examples() {
	shader_resource_t camera_resource = { 0 };
	shader_resource_t matrix_resource = { 0 };

	shader_resource_info_t camera_info = { 0 };
	camera_info.binding = 0;
	camera_info.count = 1;
	camera_info.type = SHADER_STATIC_BUFFER;
	camera_info.data_stride = sizeof(global_test_data_t);
	camera_info.data_count = 1;
	shader_resource_new(&camera_info, &camera_resource);

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

	glm_perspective(45.0f, 800.0f/600.0f, 0.1f, 100.0f, camera.camera_projection);
	glm_lookat((vec3) { 10, 10, 10 }, (vec3) { 0,0,0 }, (vec3) { 0, 1, 0 }, camera.camera_view);

}

void run_examples() {
	shader_resource_set(&shader.groups[0]->resources[0], &camera);
	shader_resource_set(&shader.groups[0]->resources[1], &model);
	shader_use(&shader);

	mesh_bind(mesh);
	mesh_draw(mesh, 4);


}

void stop_examples() {
	mesh_clear();
}