#include "vkl.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static void shader_module_new(vkl_device_t* dev, const char* path, VkShaderModule* shaderModule) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		printf("failed to open file: %s\n", path);
		exit(1);
	}

	char* data	= NULL;
	size_t size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = (char*)malloc(size);
	if (data == NULL) {
		vkl_error("failed to allocate shader module memory!\n", ERROR_FATAL);
		fclose(file);
		return;
	}

	fread(data, 1, size, file);
	fclose(file);

	VkShaderModuleCreateInfo create_info = { 0 };
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = size;
	create_info.pCode = (uint32_t*)data;

	if (vkCreateShaderModule(dev->device, &create_info, NULL, shaderModule) != VK_SUCCESS) {
		vkl_error("failed to create shader module!\n", ERROR_FATAL);
	}
	free(data);
}

void vkl_shader_new(vkl_device_t* dev, vkl_shader_t* shader, const char* vertex_path, const char* fragment_path) {
	shader_module_new(dev, vertex_path, &shader->vertex_module);
	shader_module_new(dev, fragment_path, &shader->fragment_module);
	shader->initialized = true;
}

void vkl_shader_del(vkl_device_t* dev, vkl_shader_t* shader) {
	if (!shader->initialized) return;
	vkDestroyShaderModule(dev->device, shader->vertex_module, NULL);
	vkDestroyShaderModule(dev->device, shader->fragment_module, NULL);
	memset(shader, 0, sizeof(vkl_shader_t));
}


