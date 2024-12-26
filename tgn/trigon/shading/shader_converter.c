#include "shaders.h"
#include "cmn/cmn.h"

void tgn_pipe_layout_gen(shader_res_set_t sets[MAX_DESC_SETS_IN_USE], uint32_t sets_count, VkPipelineLayout* out) {

    VkDescriptorSetLayout layouts[MAX_DESC_SETS_IN_USE] = { 0 };
    for (uint32_t i = 0; i < sets_count; ++i) {
        layouts[i] = sets[i]._layout;
    }

    VkPipelineLayoutCreateInfo layinf = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sets_count,
        .pSetLayouts = layouts
    };

    if (vkCreatePipelineLayout(
        gfx_dev()->device,
        &layinf,
        NULL,
        out) != VK_SUCCESS) {
        debug_err("failed to create pipeline layout\n");
    }
}