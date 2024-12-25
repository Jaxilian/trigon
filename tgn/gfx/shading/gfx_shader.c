#include "gfx_shading.h"

#include <vulkan/vulkan.h>


void gfx_shader_new1(gfx_shader_info_t* info, gfx_shader_t* out) {

    VkShaderModuleCreateInfo fshamod_info = {0};
    fshamod_info.sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fshamod_info.codeSize   = info->frag_bin_len;
    fshamod_info.pCode      = info->frag_bin;

    if (vkCreateShaderModule(gfx_dev()->device, &fshamod_info, NULL, &out->frag_module) != VK_SUCCESS) {
        debug_err("Failed to create shader module.\n");
        return;
    }

    fshamod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fshamod_info.codeSize = info->vert_bin_len;
    fshamod_info.pCode = info->vert_bin;

    if (vkCreateShaderModule(gfx_dev()->device, &fshamod_info, NULL, &out->vert_module) != VK_SUCCESS) {
        vkDestroyShaderModule(gfx_dev()->device, out->frag_module, NULL);
        debug_err("Failed to create shader module.\n");
        return;
    }
}

void gfx_shader_del(gfx_shader_t* in) {
    if (in->frag_module) {
        vkDestroyShaderModule(gfx_dev()->device, in->frag_module, NULL);
        in->frag_module = VK_NULL_HANDLE;
    }
    if (in->vert_module) {
        vkDestroyShaderModule(gfx_dev()->device, in->vert_module, NULL);
        in->vert_module = VK_NULL_HANDLE;
    }
}
