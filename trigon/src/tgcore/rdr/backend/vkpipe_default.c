#include "vk.h"
#include "../types/mesh.h"

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


void vkpipeline_conf_default(vkpipeline_conf_t* config) {
    config->input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    config->viewport = (VkPipelineViewportStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL
    };

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

    config->multisample = (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f
    };

    config->color_blend_attachment = (VkPipelineColorBlendAttachmentState){
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE
    };

    config->color_blend_state = (VkPipelineColorBlendStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &config->color_blend_attachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

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

    config->dynamic_state = (VkPipelineDynamicStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = default_dynamic_states_enables,
        .dynamicStateCount = default_dynamic_state_enable_count
    };


    if (config->use3D) {
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