#include "examples.h"
#include "dev/io/io.h"
#include "assets/shaders/shader.h"
#include "string.h"


static shader_resource_group_t group = { 0 };

typedef struct {
	uint32_t x;
} global_test_data_t;

void run_examples() {

	shader_resource_t resource = { 0 };
	shader_resource_info_t resource_info = { 0 };
	resource_info.binding = 0;
	resource_info.count = 1;
	resource_info.type = SHADER_STATIC_BUFFER;
	resource_info.data_stride = sizeof(global_test_data_t);
	resource_info.data_count = 1;
	shader_resource_new(&resource_info, &resource);

	shader_resource_group_info_t group_info = { 0 };
	group_info.resource_count = 1;

	memcpy(&group_info.resources[0], &resource, sizeof(shader_resource_t));
	shader_resource_group_new(&group_info, &group);
	
	shader_t shader;
	shader_info_t info = { 0 };
	info.shader_name = "vcolor";
	info.group_count = 1;
	info.groups[0] = (shader_resource_group_t*){
		&group
	};

	shader_new(&info, &shader);
}