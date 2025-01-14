#include "gfx_pipe.h"
#include "cmn/cmn.h"

void gfx_enable_alpha_blending(gfx_pipe_setting_t* config_info) {
    config_info->color_blend_attachment.blendEnable = VK_TRUE;
    config_info->color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    config_info->color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    config_info->color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    config_info->color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config_info->color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config_info->color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void gfx_pipe_default(gfx_pipe_setting_t* config_info) {
    if (!config_info) debug_err("default_pipeline_config_info() -> \"config_info\" can't be NULL");

    // Input assembly state
    config_info->input_assembly_info = (VkPipelineInputAssemblyStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // Viewport and scissor states (dynamic)
    config_info->viewport_info = (VkPipelineViewportStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = NULL, // Dynamic state
        .scissorCount = 1,
        .pScissors = NULL, // Dynamic state
    };

    // Rasterization state
    config_info->rasterization_info = (VkPipelineRasterizationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };

    // Multisample state
    config_info->multisample_info = (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    // Color blend attachment state
    config_info->color_blend_attachment = (VkPipelineColorBlendAttachmentState){
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
    };

    // Color blend state
    config_info->color_blend_info = (VkPipelineColorBlendStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &config_info->color_blend_attachment,
    };

    // Depth stencil state
    config_info->depth_stencil_info = (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    // Dynamic state
    static VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    // Set pipeline layout and subpass to VK_NULL_HANDLE and 0
    config_info->pipeline_layout = VK_NULL_HANDLE;
    config_info->subpass = 0;
}