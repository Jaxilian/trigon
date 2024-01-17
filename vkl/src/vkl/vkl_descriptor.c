#include "vkl/vkl.h"
#include <stdlib.h>
#include <memory.h>

VkDescriptorType vkl_descriptor_convert_type(vkl_shader_property_e type) {
    switch (type) {
    case SHADER_STATIC_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case SHADER_DYNAMIC_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case SHADER_SAMPLER_BUFFER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        vkl_error("unkown shader property type %s :&d", ERROR_WARNING);
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

static void convert_bindings(uint32_t count, vkl_descriptor_t descriptors[MAX_DESC_PER_SET], VkDescriptorSetLayoutBinding out[MAX_DESC_PER_SET]) {
    for (uint32_t i = 0; i < count; i++) {
        memcpy(&out[i], &descriptors[i].binding, sizeof(VkDescriptorSetLayoutBinding));
    }
}

void vkl_descriptor_new(uint32_t location, uint32_t count, vkl_shader_property_e type, vkl_shader_stage_e stage, vkl_descriptor_t* out) {
    VkDescriptorType local_type = { 0 };
    VkShaderStageFlags _stage = { 0 };

    switch (type) {
    case SHADER_STATIC_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_DYNAMIC_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        _stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_SAMPLER_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        _stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    default:
        vkl_error("unkown shader property type %s :&d", ERROR_FATAL);
        break;
    }

    switch (stage) {
    case SHADER_STAGE_VERTEX:
        _stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_STAGE_FRAGMENT:
        _stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    default:
        break;
    }

    out->binding = (VkDescriptorSetLayoutBinding){
        .binding = location,
        .descriptorType = local_type,
        .descriptorCount = count,
        .stageFlags = _stage,
        .pImmutableSamplers = NULL
    };
    out->count = count;
    out->location = location;
    out->type = type;
    out->initialized = true;
}

static vkl_desc_create_pool(vkl_device_t* device, vkl_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count, vkl_descriptor_set_t* in) {
    VkDescriptorPoolSize pool_sizes[MAX_DESC_PER_SET] = { 0 };

    for (uint32_t i = 0; i < count; i++) {
        pool_sizes[i] = (VkDescriptorPoolSize){
              .type = vkl_descriptor_convert_type(descriptors[i].type),
              .descriptorCount = descriptors[i].count,
        };
    }

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = count,
        .pPoolSizes = pool_sizes,
        .maxSets = 1,
    };

    if (vkCreateDescriptorPool(
        device->device,
        &pool_info,
        NULL,
        &in->pool) != VK_SUCCESS) {
        vkl_error("failed to create descriptor set pool!\n", ERROR_FATAL);
    }
}

void vkl_descriptor_set_new(vkl_device_t* device, vkl_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count, vkl_descriptor_set_t* out) {
    out->descriptor_count = count;
    memcpy(out->descriptors, descriptors, sizeof(descriptors));

    VkDescriptorSetLayoutBinding bindings[MAX_DESC_PER_SET] = { 0 };
    convert_bindings(count, descriptors, bindings);

    VkDescriptorSetLayoutCreateInfo layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = count,
      .pBindings = bindings,
    };
    vkCreateDescriptorSetLayout(device->device, &layout_info, NULL, &out->set_layout);

    vkl_desc_create_pool(device, descriptors, count, out);

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = out->pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &out->set_layout,
    };

    if (vkAllocateDescriptorSets(
        device->device,
        &alloc_info,
        &out->set) != VK_SUCCESS) {
        vkl_error("failed to allocate descriptor sets!\n", ERROR_FATAL);
    }

    out->initialized = true;
}

void vkl_descriptor_set_del(vkl_device_t* device, vkl_descriptor_set_t* in) {
    vkDestroyDescriptorPool(device->device, in->pool, NULL);
    vkDestroyDescriptorSetLayout(device->device, in->set_layout, NULL);
}

void vkl_descriptor_set_update_ubo(vkl_device_t* device, vkl_descriptor_set_t* in, uint32_t descriptor_id, vkl_buffer_t* buffers[MAX_DESC_PER_SET], uint32_t count) {
    if (descriptor_id > in->descriptor_count && count < 0) {
        vkl_error("index out of bounds!", ERROR_FATAL);
    }
    
    vkl_descriptor_t* descriptor = &in->descriptors[descriptor_id];

    if (count > in->descriptor_count && count < 0) {
        vkl_error("index out of bounds!", ERROR_FATAL);
    }

    VkDescriptorBufferInfo bufferInfos[MAX_DESC_PER_SET];
    VkWriteDescriptorSet descriptorWrites[MAX_DESC_PER_SET];

    for (uint32_t i = 0; i < count; i++) {
        vkl_buffer_t* buffer = buffers[i];
        bufferInfos[i] = (VkDescriptorBufferInfo){
            .buffer = buffer->buffer,
            .offset = 0,
            .range = buffer->size
        };

        descriptorWrites[i] = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = in->set,
            .dstBinding = descriptor->location,
            .descriptorCount = 1,
            .descriptorType = vkl_descriptor_convert_type(descriptor->type),
            .pBufferInfo = &bufferInfos[i],
        };
    }

    vkUpdateDescriptorSets(device->device, count, descriptorWrites, 0, NULL);
}

