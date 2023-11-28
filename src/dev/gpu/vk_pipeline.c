#include "vk.h"

void vk_pipeline_config_set_default(vk_pipeline_config_t* config) {
    // Input Assembly
    config->input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };


    // Viewport State
    config->viewport = (VkPipelineViewportStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL
    };

    // Rasterization State
    config->rasterization = (VkPipelineRasterizationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f
    };

    // Multisample State
    config->multisample = (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f
    };

    // Color Blend Attachment State
    config->color_blend_attachment = (VkPipelineColorBlendAttachmentState){
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE
    };

    // Color Blend State
    config->color_blend_state = (VkPipelineColorBlendStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &config->color_blend_attachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    // Depth Stencil State
    config->depth_stencil = (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
        .stencilTestEnable = VK_FALSE,
        .front = {0},
        .back = {0}
    };

    // Dynamic State
    config->dynamic_state = (VkPipelineDynamicStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = default_dynamic_states_enables,
        .dynamicStateCount = default_dynamic_state_enable_count
    };

    config->vertex_attribute_count = default_vertex_attribute_count;
    config->vertex_binding_count = default_vertex_binding_count;
    config->vertex_attribute = (VkVertexInputAttributeDescription*)default_vertex_attributes;
    config->vertex_binding = (VkVertexInputBindingDescription*)default_vertex_binding;

    config->subpass = 0;
}

uint32_t get_next_pipeline() {
    for (uint32_t i = 0; i < MAX_SHADERS; ++i) {
        if (ctx->pipelines[i].initialized) continue;
        return i;
    }
    validate(false, "max pipeline count (%d) reached", MAX_SHADERS);
    exit(1);
}

vk_pipeline_t* get_pipeline(uint32_t id) {
    if (!ctx->pipelines[id].initialized) {
        return NULL;
    }
    return &ctx->pipelines[id];
}

void vk_pipeline_rebuild() {
    vk_pipeline_config_t default_config = { 0 };
    vk_pipeline_config_set_default(&default_config);

    for (uint32_t i = 0; i < MAX_SHADERS; i++) {
        vk_pipeline_t* pipeline = get_pipeline(i);
        if (!pipeline) continue;

        VkPipelineLayout layout = pipeline->layout;
        vk_shader_t* shader = pipeline->shader;
        vk_pipeline_config_t* config = pipeline->config;

        if (config == NULL)  config = &default_config;

        vkDestroyPipeline(ctx->device.device, pipeline->instance, NULL);

        VkPipelineShaderStageCreateInfo shaderStages[2] = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = shader->vertex_module, .pName = "main"},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = shader->fragment_module, .pName = "main"}
        };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexAttributeDescriptionCount = config->vertex_attribute_count,
            .vertexBindingDescriptionCount = config->vertex_binding_count,
            .pVertexAttributeDescriptions = config->vertex_attribute,
            .pVertexBindingDescriptions = config->vertex_binding
        };

        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &config->input_assembly,
            .pViewportState = &config->viewport,
            .pRasterizationState = &config->rasterization,
            .pMultisampleState = &config->multisample,
            .pColorBlendState = &config->color_blend_state,
            .pDepthStencilState = &config->depth_stencil,
            .pDynamicState = &config->dynamic_state,
            .layout = layout,
            .renderPass = ctx->swapchain.renderpass,
            .subpass = config->subpass,
            .basePipelineIndex = -1,
            .basePipelineHandle = VK_NULL_HANDLE
        };

        bool success = false;
        success = vkCreateGraphicsPipelines(
            ctx->device.device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            NULL,
            &pipeline->instance) == VK_SUCCESS;
        validate(success, "failed to create pipeline\n");
        pipeline->initialized = true;
    }
}

void generate_pipeline_layout(vk_descriptor_set_t sets[MAX_DESCRIPTOR_SETS_IN_USE], uint32_t sets_count, VkPipelineLayout* out) {
    VkDescriptorSetLayout layouts[MAX_DESCRIPTOR_SETS_IN_USE] = { 0 };
    for (uint32_t i = 0; i < sets_count; ++i) {
        layouts[i] = sets[i].set_layout;
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sets_count,
        .pSetLayouts = layouts
    };

    validate(
        vkCreatePipelineLayout(
            ctx->device.device,
            &pipeline_layout_info,
            NULL,
            out) == VK_SUCCESS,
        "failed to create pipeline layout!\n");
}

uint32_t vk_pipeline_new(vk_descriptor_set_t sets[MAX_DESCRIPTOR_SETS_IN_USE], uint32_t sets_count, vk_shader_t* shader, vk_pipeline_config_t* config) {
    uint32_t id = get_next_pipeline();
    vk_pipeline_t* pipeline = get_pipeline(id);

    generate_pipeline_layout(sets, sets_count, &pipeline->layout);

    pipeline->config = config;
    pipeline->shader = shader;

    vk_pipeline_config_t default_config = { 0 };
    if (config == NULL) {
        vk_pipeline_config_set_default(&default_config);
        config = &default_config;
    }

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = shader->vertex_module, .pName = "main"},
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = shader->fragment_module, .pName = "main"}
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexAttributeDescriptionCount = config->vertex_attribute_count,
        .vertexBindingDescriptionCount = config->vertex_binding_count,
        .pVertexAttributeDescriptions = config->vertex_attribute,
        .pVertexBindingDescriptions = config->vertex_binding
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &config->input_assembly,
        .pViewportState = &config->viewport,
        .pRasterizationState = &config->rasterization,
        .pMultisampleState = &config->multisample,
        .pColorBlendState = &config->color_blend_state,
        .pDepthStencilState = &config->depth_stencil,
        .pDynamicState = &config->dynamic_state,
        .layout = pipeline->layout,
        .renderPass = ctx->swapchain.renderpass,
        .subpass = config->subpass,
        .basePipelineIndex = -1,
        .basePipelineHandle = VK_NULL_HANDLE
    };

    bool success = vkCreateGraphicsPipelines(
        ctx->device.device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        NULL,
        &pipeline->instance) == VK_SUCCESS;
    validate(success, "failed to create pipeline\n");
    pipeline->initialized = true;

    return id;
}

void vk_pipeline_del(uint32_t id) {
    vk_pipeline_t* pipeline = get_pipeline(id);
    if (!pipeline) return;

    vkDeviceWaitIdle(ctx->device.device);
    vkDestroyPipelineLayout(ctx->device.device, pipeline->layout, NULL);
    vkDestroyPipeline(ctx->device.device, pipeline->instance, NULL);
    pipeline->initialized = false;
}

void vk_pipeline_clear() {
    for (uint32_t i = 0; i < MAX_SHADERS; i++) {
        vk_pipeline_del(i);
    }
}

vk_pipeline_t* vk_pipeline_get(uint32_t id) {
    return get_pipeline(id);
}

void vk_pipeline_bind(vk_pipeline_t* pipeline) {
    vkCmdBindPipeline(vk_command_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->instance);
}