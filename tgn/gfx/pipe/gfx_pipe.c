#include "gfx_pipe.h"
#include "cmn/cmn.h"

void gfx_shader_new(const uint32_t* bin, size_t bin_size, VkShaderModule* mod) {
	VkShaderModuleCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = bin_size,
		.pCode = bin,
	};

	if (vkCreateShaderModule(gfx_dev()->device, &info, NULL, mod) != VK_SUCCESS) {
		debug_err("Failed to create shader module.\n");
		return;
	}

}