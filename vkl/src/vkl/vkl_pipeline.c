#include "vkl/vkl.h"
#include "common.h"

/* ------------ SETTINGS ------------------*/

#define default_dynamic_state_enable_count 2
static const VkDynamicState default_dynamic_states_enables[] = {
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
};

#define vertex3_binding_count 1
#define vertex3_attribute_count 4

#define vertex2_binding_count 1
#define vertex2_attribute_count 2

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

static const VkVertexInputBindingDescription vertex2_binding[vertex2_binding_count] = {
    {
    .binding = 0,
    .stride = sizeof(vertex2_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    }
};

static const VkVertexInputAttributeDescription vertex2_attributes[vertex2_attribute_count] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(vertex2_t, position)
    },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(vertex2_t, uv)
    }
};


/* ---------- LOGIC --------------*/

void vkl_pipeline_config_set_default(vkl_pipeline_config_t* config) {
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
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
        .pDynamicStates     = default_dynamic_states_enables,
        .dynamicStateCount  = default_dynamic_state_enable_count
    };

    if (config->use_for_3D) {
        config->vertex_attribute_count = vertex3_attribute_count;
        config->vertex_binding_count = vertex3_binding_count;
        config->vertex_attribute = (VkVertexInputAttributeDescription*)vertex3_attributes;
        config->vertex_binding = (VkVertexInputBindingDescription*)vertex3_binding;
        config->subpass = 0;
    }
    else {
        config->vertex_attribute_count = vertex2_attribute_count;
        config->vertex_binding_count = vertex2_binding_count;
        config->vertex_attribute = (VkVertexInputAttributeDescription*)vertex2_attributes;
        config->vertex_binding = (VkVertexInputBindingDescription*)vertex2_binding;
        config->subpass = 0;
    }
   
}

void vkl_pipeline_rebuild(vkl_pipeline_t* pipeline, vkl_device_t* device, vkl_swapchain_t* swapchain) {
    vkl_pipeline_config_t default_config = { 0 };
    vkl_pipeline_config_set_default(&default_config);

    for (uint32_t i = 0; i < MAX_SHADERS; i++) {
        if (!pipeline) continue;

        VkPipelineLayout layout = pipeline->layout;
        vkl_shader_t* shader = pipeline->shader;
        vkl_pipeline_config_t* config = pipeline->config;

        if (config == NULL)  config = &default_config;

        vkDestroyPipeline(device->device, pipeline->instance, NULL);

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
            .renderPass = swapchain->renderpass,
            .subpass = config->subpass,
            .basePipelineIndex = -1,
            .basePipelineHandle = VK_NULL_HANDLE
        };
        
        if (vkCreateGraphicsPipelines(
            device->device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            NULL,
            &pipeline->instance) != VK_SUCCESS) {
            vkl_error("failed to create pipeline\n", ERROR_FATAL);
        }

        pipeline->initialized = true;
    }
}

static void generate_pipeline_layout(vkl_device_t* device, vkl_descriptor_set_t* sets[MAX_DESC_SETS_IN_USE], uint32_t sets_count, VkPipelineLayout* out) {
    VkDescriptorSetLayout layouts[MAX_DESC_SETS_IN_USE] = { 0 };
    for (uint32_t i = 0; i < sets_count; ++i) {
        layouts[i] = sets[i]->set_layout;
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sets_count,
        .pSetLayouts = layouts
    };

    if (vkCreatePipelineLayout(
            device->device,
            &pipeline_layout_info,
            NULL,
            out) != VK_SUCCESS) {
        vkl_error("failed to create pipeline layout!\n", ERROR_FATAL);
    }
}

void vkl_pipeline_new(vkl_pipeline_t* pipeline, vkl_pipeline_refs_t* references, vkl_pipeline_config_t* config) {

    generate_pipeline_layout(references->device, references->sets, references->sets_count, &pipeline->layout);

    pipeline->config = config;
    pipeline->shader = references->shader;

    vkl_pipeline_config_t default_config = { 0 };
    if (config == NULL) {
        vkl_pipeline_config_set_default(&default_config);
        config = &default_config;
    }

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = references->shader->vertex_module, .pName = "main"},
        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = references->shader->fragment_module, .pName = "main"}
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
        .renderPass = references->swapchain->renderpass,
        .subpass = config->subpass,
        .basePipelineIndex = -1,
        .basePipelineHandle = VK_NULL_HANDLE
    };

    if (vkCreateGraphicsPipelines(
        references->device->device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        NULL,
        &pipeline->instance) == VK_SUCCESS) {
        vkl_error("failed to create pipeline\n", ERROR_FATAL);
    }

    pipeline->initialized = true;
}

void vkl_pipeline_del(vkl_device_t* device, vkl_pipeline_t* pipeline) {
    vkDeviceWaitIdle(device->device);
    vkDestroyPipelineLayout(device->device, pipeline->layout, NULL);
    vkDestroyPipeline(device->device, pipeline->instance, NULL);
    pipeline->initialized = false;
}

void vkl_pipeline_bind(vkl_state_t* state, vkl_pipeline_t* pipeline) {
    vkCmdBindPipeline(vkl_state_command_buffer(state), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->instance);
}

