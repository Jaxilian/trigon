#include "gfx_pipe.h"
#include "cmn/cmn.h"
#include <string.h>

#define MAX_DESC_SETS_IN_USE  4

static VkDescriptorType convert_shader_buffer_type(SHADER_BUFFER_TYPE type)
{
    switch (type) {
    case SHADER_BUFFER_TYPE_STATIC:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case SHADER_BUFFER_TYPE_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case SHADER_BUFFER_TYPE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        debug_err("unknown shader buffer type: %d\n", (int)type);
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

void gfx_descriptor_setup(shader_res_t* res)
{
    VkDescriptorType    desc_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    VkShaderStageFlags  stage_flags = 0;

    switch (res->buffer_type) {
    case SHADER_BUFFER_TYPE_STATIC:
        desc_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_BUFFER_TYPE_DYNAMIC:
        desc_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_BUFFER_TYPE_SAMPLER:
        desc_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    default:
        debug_err("unknown shader buffer type: %d\n", (int)res->buffer_type);
        break;
    }

    res->_binding = (VkDescriptorSetLayoutBinding){
        .binding = res->location,
        .descriptorType = desc_type,
        .descriptorCount = res->count,
        .stageFlags = stage_flags,
        .pImmutableSamplers = NULL
    };
}

void gfx_desc_pool_setup(gfx_pipe_t* pipe,
    shader_res_set_t sets[MAX_DESC_SETS_IN_USE],
    uint32_t set_count)
{
    if (pipe->descriptor_pool != VK_NULL_HANDLE) return;

    VkDevice dev = gfx_dev()->device;
    VkDescriptorPoolSize pool_sizes[SHADER_BUFFER_TYPE_COUNT] = { 0 };
    uint32_t pool_size_count = 0;

    for (uint32_t i = 0; i < set_count; ++i) {
        for (uint32_t j = 0; j < sets[i].resource_count; ++j) {
            shader_res_t* res = &sets[i].resources[j];
            gfx_descriptor_setup(res);

            if (pool_sizes[res->buffer_type].descriptorCount == 0) {
                pool_size_count++;
            }

            pool_sizes[res->buffer_type].type = convert_shader_buffer_type(res->buffer_type);
            pool_sizes[res->buffer_type].descriptorCount += 1;
        }
    }

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = pool_size_count,
        .pPoolSizes = pool_sizes,
        .maxSets = MAX_DESC_SETS_IN_USE
    };

    if (vkCreateDescriptorPool(dev, &pool_info, NULL, &pipe->descriptor_pool) != VK_SUCCESS) {
        debug_err("failed to create descriptor pool\n");
    }

    debug_log("Pool setup!\n");
}

void gfx_desc_pool_alloc(gfx_pipe_t* pipe, shader_res_set_t sets[MAX_DESC_SETS_IN_USE], uint32_t set_count)
{

    debug_log("\nCRASH HERE ; gfx_descriptor.c\n\n");

    VkDevice dev = gfx_dev()->device;
    if (!dev || pipe->descriptor_pool == VK_NULL_HANDLE) {
        debug_err("missing arguments for descriptor allocation\n");
        return;
    }

    VkDescriptorSetLayout layouts[MAX_DESC_SETS_IN_USE] = { 0 };
    for (uint32_t i = 0; i < set_count; i++) {
        layouts[i] = sets[i]._layout;
    }

    VkDescriptorSet tmp_sets[MAX_DESC_SETS_IN_USE] = { 0 };

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pipe->descriptor_pool,
        .descriptorSetCount = set_count,
        .pSetLayouts = layouts
    };

    if (vkAllocateDescriptorSets(dev, &alloc_info, tmp_sets) != VK_SUCCESS) {
        debug_err("failed to allocate descriptor sets\n");
        return;
    }

    for (uint32_t i = 0; i < set_count; i++) {
        sets[i]._set = tmp_sets[i];
    }
}

void gfx_pipe_layout_setup(shader_res_set_t sets[MAX_DESC_SETS_IN_USE],
    uint32_t sets_count,
    VkPipelineLayout* out_layout)
{
    VkDescriptorSetLayout layouts[MAX_DESC_SETS_IN_USE] = { 0 };
    for (uint32_t i = 0; i < sets_count; ++i) {
        layouts[i] = sets[i]._layout;
    }

    VkPipelineLayoutCreateInfo lay_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sets_count,
        .pSetLayouts = layouts
    };

    if (vkCreatePipelineLayout(gfx_dev()->device, &lay_info, NULL, out_layout) != VK_SUCCESS) {
        debug_err("failed to create pipeline layout\n");
    }
   
}
