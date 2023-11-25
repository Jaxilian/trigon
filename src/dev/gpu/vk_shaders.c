#include "vk.h"
#include "dev/io/io.h"

static void shader_module_new(cstr path, VkShaderModule* shaderModule) {

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

void vk_shader_new(vk_shader_t* shader, cstr vertex_path, cstr fragment_path) {
	shader_module_new(vertex_path, &shader->vertex_module);
	shader_module_new(fragment_path, &shader->fragment_module);
	shader->initialized = true;
}

void vk_shader_del(vk_shader_t* shader) {
	if (!shader->initialized) return;
	vkDestroyShaderModule(ctx->device.device, shader->vertex_module, NULL);
	vkDestroyShaderModule(ctx->device.device, shader->fragment_module, NULL);
	shader->initialized = false;
}