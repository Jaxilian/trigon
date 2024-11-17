#include "gfx/shading/gfx_shading.h"
#include <stdlib.h>
#include <memory.h>



void gfx_pipe_del(gfx_shader_t* in) {
    VkDevice dev = gfx_dev()->device;
    if (!dev)return;

    if (in->vert_module) {
        vkDestroyShaderModule(dev, in->vert_module, NULL);
        in->vert_module = NULL;
    }

    if (in->frag_module) {
        vkDestroyShaderModule(dev, in->frag_module, NULL);
        in->frag_module = NULL;
    }

    if (in->pipeline) {
        vkDestroyPipeline(dev, in->pipeline, NULL);
        in->pipeline = NULL;
    }
}


void gfx_pipe_add(
    gfx_shader_t* in,
    const gfx_pipeline_layout_t*layout,
    const gfx_pipeline_info_t*  pinfo,
    const gfx_viewport_info_t*  vinfo,
    const win_t*                window
) {
    VkDevice dev = gfx_dev()->device;
    if (!dev)return;

    if (!pinfo)debug_err("gfx_pipe_add()->pinfo can't be null\n");
    if (!in)debug_err("gfx_pipe_add()->in can't be null\n");
    if (!layout)debug_err("gfx_pipe_add()->layout can't be null\n");
    if (!window)debug_err("gfx_pipe_add()->window can't be null\n");

    VkResult result;
    // PIPELINE CREATION

    VkPipelineShaderStageCreateInfo shader_stages[2] = {
       {
           .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           .stage = VK_SHADER_STAGE_VERTEX_BIT,
           .module = in->vert_module,
           .pName = "main",
       },
       {
           .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
           .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
           .module = in->frag_module,
           .pName = "main",
       }
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = pinfo->binding_description_count,
        .pVertexBindingDescriptions = pinfo->binding_descriptions,
        .vertexAttributeDescriptionCount = pinfo->attribute_description_count,
        .pVertexAttributeDescriptions = pinfo->attribute_descriptions,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &pinfo->input_assembly_info,
        .pViewportState = &pinfo->viewport_info,
        .pRasterizationState = &pinfo->rasterization_info,
        .pMultisampleState = &pinfo->multisample_info,
        .pColorBlendState = &pinfo->color_blend_info,
        .pDepthStencilState = &pinfo->depth_stencil_info,
        .pDynamicState = &pinfo->dynamic_state_info,
        .layout = pinfo->pipeline_layout,
        .renderPass = window->swapchain.renderpass,
        .subpass = pinfo->subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    result = vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &in->pipeline);
    if (result != VK_SUCCESS) {
       debug_err("Failed to create graphics pipeline.\n");
    }

    vkDestroyShaderModule(dev, in->vert_module, NULL);
    vkDestroyShaderModule(dev, in->frag_module, NULL);
    in->vert_module = NULL;
    in->frag_module = NULL;

    // CREATE DESCRIPTOR SET

    VkDescriptorPool descriptor_pool;
    gfx_desc_pool_new(
        layout->shader_layouts,
        layout->shader_layout_count,
        1, // Number of descriptor sets per layout
        &descriptor_pool
    );


    return;
}

