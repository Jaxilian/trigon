#include "gfx_pipe.h"
#include "cmn/cmn.h"
#include "vertex.h"

#define vertex3_binding_count 1
#define vertex3_attribute_count 4

static const VkVertexInputBindingDescription vertex3_binding[vertex3_binding_count] = {
    {
        .binding = 0,
        .stride = sizeof(vertex3_t),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    }
};

static const VkVertexInputAttributeDescription vertex3_attributes[vertex3_attribute_count] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex3_t, position)
    },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex3_t, normal)
    },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex3_t, color)
    },
    {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(vertex3_t, uv)
    }
};

static const VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
};

void gfx_shader_new(const uint32_t* bin, size_t bin_size, VkShaderModule* mod) {
    if (!gfx_dev()) return;

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

void gfx_pipe_new(gfx_pipe_info_t* info, gfx_pipe_t* out) {

    VkResult result;
    VkPipelineShaderStageCreateInfo shader_stages[2] = {
       {
           .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           .stage = VK_SHADER_STAGE_VERTEX_BIT,
           .module = info->vertmod,
           .pName = "main",
       },
       {
           .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
           .module = info->fragmod,
           .pName = "main",
       }
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamic_states
    };


    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0, //vertex3_binding_count,
        .pVertexBindingDescriptions =  NULL,//vertex3_binding,
        .vertexAttributeDescriptionCount = 0,//vertex3_attribute_count,
        .pVertexAttributeDescriptions = NULL, //vertex3_attributes,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &info->settings.input_assembly_info,
        .pViewportState = &info->settings.viewport_info,
        .pRasterizationState = &info->settings.rasterization_info,
        .pMultisampleState = &info->settings.multisample_info,
        .pColorBlendState = &info->settings.color_blend_info,
        .pDepthStencilState = &info->settings.depth_stencil_info,
        .pDynamicState = &dynamic_state_info,
        .layout = info->settings.pipeline_layout,
        .renderPass = info->window->swapchain.renderpass,
        .subpass = info->settings.subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    result = vkCreateGraphicsPipelines(gfx_dev()->device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &out->pipeline);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create graphics pipeline.\n");
    }
 
}