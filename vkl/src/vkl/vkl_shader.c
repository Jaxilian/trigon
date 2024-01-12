#include "vkl.h"
#include <stdlib.h>
#include <memory.h>

/*
static void shader_module_new(const char* path, VkShaderModule* shaderModule) {

	size_t size;
	char* data = file_open(path, &size);
	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = (uint32_t*)data;
	bool success = false;
	success = vkCreateShaderModule(ctx->device.device, &createInfo, NULL, shaderModule) == VK_SUCCESS;
	validate(success, "failed to create shader module");
	file_close();
}

void vk_shader_new(vkl_shader_t* shader, const char* vertex_path, const char* fragment_path) {
	shader_module_new(vertex_path, &shader->vertex_module);
	shader_module_new(fragment_path, &shader->fragment_module);
	shader->initialized = true;
}

void vk_shader_del(vkl_shader_t* shader) {
	if (!shader->initialized) return;
	vkDestroyShaderModule(ctx->device.device, shader->vertex_module, NULL);
	vkDestroyShaderModule(ctx->device.device, shader->fragment_module, NULL);
	memset(shader, 0, sizeof(vk_shader_t));
}

void vk_shader_clear() {
	for (uint32_t i = 0; i < MAX_SHADERS; i++) {
		if (!ctx->shaders[i].initialized) continue;
		vk_shader_del(&ctx->shaders[i]);
	}
}
*/