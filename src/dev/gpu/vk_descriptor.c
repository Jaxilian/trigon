#include "vk.h"

static VkDescriptorType convert_type(shader_property_e type) {
    switch (type) {
    case SHADER_STATIC_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case SHADER_DYNAMIC_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case SHADER_SAMPLER_BUFFER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        validate(false, "unkown shader property type %s :&d", __FILE__, __LINE__);
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

static void convert_bindings(uint32_t count, vk_descriptor_t descriptors[MAX_DESC_PER_SET], VkDescriptorSetLayoutBinding out[MAX_DESC_PER_SET]) {
    for (uint32_t i = 0; i < count; i++) {
        memcpy(&out[i], &descriptors[i].binding, sizeof(VkDescriptorSetLayoutBinding));
    }
}

void vk_descriptor_new(uint32_t location, uint32_t count, shader_property_e type, vk_descriptor_t* out) {
    VkDescriptorType local_type = { 0 };
    VkShaderStageFlags stage = { 0 };

    switch (type) {
    case SHADER_STATIC_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_DYNAMIC_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case SHADER_SAMPLER_BUFFER:
        local_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    default:
        validate(false, "unkown shader property type %s :&d", __FILE__, __LINE__);
        break;
    }

    out->binding = (VkDescriptorSetLayoutBinding){
        .binding = location,
        .descriptorType = local_type,
        .descriptorCount = count,
        .stageFlags = stage,
        .pImmutableSamplers = NULL
    };
    out->count = count;
    out->location = location;
    out->type = type;
}

static vk_desc_create_pool(vk_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count, vk_descriptor_set_t* in) {
    VkDescriptorPoolSize pool_sizes[MAX_DESC_PER_SET] = { 0 };

    for (uint32_t i = 0; i < count; i++) {
        pool_sizes[i] = (VkDescriptorPoolSize){
              .type = convert_type(descriptors[i].type),
              .descriptorCount = descriptors[i].count,
        };
    }

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = count,
        .pPoolSizes = pool_sizes,
        .maxSets = 1,
    };

    validate(
        vkCreateDescriptorPool(
            ctx->device.device,
            &pool_info,
            NULL,
            &in->pool) == VK_SUCCESS,
        "failed to create descriptor set pool!\n"
    );
}

void vk_descriptor_set_new(vk_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count,  vk_descriptor_set_t* out) {
    out->descriptor_count = count;
    memcpy(out->descriptors, descriptors, sizeof(descriptors));
    
    VkDescriptorSetLayoutBinding bindings[MAX_DESC_PER_SET] = { 0 };
    convert_bindings(count, descriptors, bindings);

    VkDescriptorSetLayoutCreateInfo layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = count,
      .pBindings = bindings,
    };
    vkCreateDescriptorSetLayout(ctx->device.device, &layout_info, NULL, &out->set_layout);

    vk_desc_create_pool(descriptors, count, out);

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = out->pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &out->set_layout,
    };
    validate(
        vkAllocateDescriptorSets(
            ctx->device.device,
            &alloc_info,
            &out->set
        ) == VK_SUCCESS,
        "failed to allocate descriptor sets!\n");
}

void vk_descriptor_set_del(vk_descriptor_set_t* in) {
    vkDestroyDescriptorPool(ctx->device.device, in->pool, NULL);
    vkDestroyDescriptorSetLayout(ctx->device.device, in->set_layout, NULL);
}

void vk_descriptor_set_update_ubo(vk_descriptor_set_t* in, uint32_t descriptor_id, vk_buffer_t* buffers[MAX_DESC_PER_SET], uint32_t count) {
    validate(
        descriptor_id <= in->descriptor_count &&
        count >= 0
        , "index out of bounds!"
    );

    vk_descriptor_t* descriptor = &in->descriptors[descriptor_id];
    validate(
        count <= descriptor->count &&
        count >= 0
        , "index out of bounds!"
    );

    VkDescriptorBufferInfo bufferInfos[MAX_DESC_PER_SET];
    VkWriteDescriptorSet descriptorWrites[MAX_DESC_PER_SET];

    for (uint32_t i = 0; i < count; i++) {
        vk_buffer_t* buffer = buffers[i];
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
            .descriptorType = convert_type(descriptor->type),
            .pBufferInfo = &bufferInfos[i],
        };
    }

    vkUpdateDescriptorSets(ctx->device.device, count, descriptorWrites, 0, NULL);
}