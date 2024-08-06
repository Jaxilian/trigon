#include "shader.h"
#include "trigon/core/dev/vk/vk.h"
#include <string.h>

static cstr_t frag_ext			= "_frag.spv";
static cstr_t vert_ext			= "_vert.spv";
static cstr_t shader_location	= "data/shaders/";

static void shader_module_new(cstr_t path, VkShaderModule* mod) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		printf("failed to open file: %s\n", path);
		exit(1);
	}

	char* data = NULL;
	size_t size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = (char*)malloc(size);
	cassert(data, "failed to allocate memory for shader module!\n");

	fread(data, 1, size, file);
	fclose(file);

	VkShaderModuleCreateInfo create_info = { 0 };
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = size;
	create_info.pCode = (uint32_t*)data;

	cassert(
		vkCreateShaderModule(
			vkctx_get()->dev.device,
			&create_info,
			NULL,
			mod
		) == VK_SUCCESS, 
		"failed to create shader module\n"
	);

	free(data);
}

typedef struct {
	VkShaderModule vertmod;
	VkShaderModule fragmod;
} vkpipeline_t;

void shader_new(shader_info_t* info) {
	vkpipeline_t pipe = { 0 };

	u64 id = murmur3_64(info->name, strlen(info->name), 0) % 60;
	printf("%zu\n", id);
	/*
	char module_name[1024] = { 0 };
	
	strcpy(module_name, shader_location);
	strcat(module_name, info->name);
	strcat(module_name, vert_ext);
	shader_module_new(module_name, &pipe.vertmod);

	strcpy(module_name, shader_location);
	strcat(module_name, info->name);
	strcat(module_name, frag_ext);
	shader_module_new(module_name, &pipe.fragmod);
	*/



}

