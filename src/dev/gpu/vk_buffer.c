#include "vk.h"
#include <memory.h>

void vk_buffer_new(size_t stride, uint32_t count, vk_buffer_t* out) {

    VkBufferCreateInfo buffer_info = {
       .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
       .size = stride * count,
       .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, // TODO: Support multiple types!
       .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    vkCreateBuffer(ctx->device.device, &buffer_info, NULL, &out->buffer);
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(ctx->device.device, out->buffer, &mem_req);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vk_fetch_memory_type(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };
    vkAllocateMemory(ctx->device.device, &alloc_info, NULL, &out->memory);
    vkBindBufferMemory(ctx->device.device, out->buffer, out->memory, 0);

    out->size = stride * count;
    out->stride = stride;
    out->count = count;
    out->initialized = true;
}

void vk_buffer_resize(vk_buffer_t* buffer, uint32_t new_count) {
    vkDeviceWaitIdle(ctx->device.device);

    vkDestroyBuffer(ctx->device.device, buffer->buffer, NULL);
    vkFreeMemory(ctx->device.device, buffer->memory, NULL);

    buffer->size = buffer->stride * new_count;
    buffer->count = new_count;

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = buffer->size,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    vkCreateBuffer(ctx->device.device, &buffer_info, NULL, &buffer->buffer);

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(ctx->device.device, buffer->buffer, &mem_req);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vk_fetch_memory_type(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };
    vkAllocateMemory(ctx->device.device, &alloc_info, NULL, &buffer->memory);
    vkBindBufferMemory(ctx->device.device, buffer->buffer, buffer->memory, 0);
}

void vk_buffer_set(vk_buffer_t* buffer, void* data) {
    if (!buffer->initialized) return;

    void* mapped_memory;
    VkResult result = vkMapMemory(ctx->device.device, buffer->memory, 0, buffer->size, 0, &mapped_memory);
    if (result == VK_SUCCESS) {
        memcpy(mapped_memory, data, buffer->size);
        vkUnmapMemory(ctx->device.device, buffer->memory);
    }
    else {
        validate(false, "failed to set data to vulkan buffer!\n");
    }
}

void vk_buffer_del(vk_buffer_t* buffer) {
    if (!buffer->initialized) return;

    vkFreeMemory(ctx->device.device, buffer->memory, NULL);
    vkDestroyBuffer(ctx->device.device, buffer->buffer, NULL);
    buffer->memory = VK_NULL_HANDLE;
    buffer->buffer = VK_NULL_HANDLE;
    buffer->initialized = false;
}

void vk_buffer_clear(vk_buffer_t* arr, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        vk_buffer_del(&arr[i]);
    }
}