#include "gfx_types.h"
#include "cmn/cmn.h"
#include <memory.h>

VkResult gfx_buffer_new(gfx_buffer_info_t* info, gfx_buffer_t* out) {

    VkBufferCreateInfo buff_info = { 0 };
    buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buff_info.size = info->size; // Buffer size in bytes
    buff_info.usage = info->usage; // Buffer usage flags
    buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive access

    out->size = info->count * info->stride;
    out->stride = info->stride;
    out->count = info->count;
    out->usage = info->usage;

    VkResult result = vkCreateBuffer(gfx_dev()->device, &buff_info, NULL, &out->buffer);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create buffer!\n");
        return result;
    }

    VkMemoryRequirements memreq = { 0 };
    vkGetBufferMemoryRequirements(gfx_dev()->device, out->buffer, &memreq);

    VkMemoryAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memreq.size;
    alloc_info.memoryTypeIndex = gfx_find_memory_type(
        gfx_device_get(NULL)->physical,
        memreq.memoryTypeBits,
        info->properties
    );

    result = vkAllocateMemory(gfx_dev()->device, &alloc_info, NULL, &out->memory);
    if (result != VK_SUCCESS) {
        debug_err("Failed to allocate buffer memory!\n");
        vkDestroyBuffer(gfx_dev()->device, out->buffer, NULL);
        return result;
    }

    vkBindBufferMemory(gfx_dev()->device, out->buffer, out->memory, 0);

    return VK_SUCCESS;
}

void vkl_buffer_set(gfx_buffer_t* buffer, void* data) {
    if (!buffer || !data) return;

    void* mapped_memory;
    if (vkMapMemory(gfx_dev()->device, buffer->memory, 0, buffer->size, 0, &mapped_memory) != VK_SUCCESS) {
        debug_err("Failed to map Vulkan buffer memory!\n");
    }

    memcpy(mapped_memory, data, buffer->size);
    vkUnmapMemory(gfx_dev()->device, buffer->memory);
}

void gfx_buffer_del(gfx_buffer_t* buffer) {
    vkDestroyBuffer(gfx_dev()->device, buffer->buffer, NULL);
    vkFreeMemory(gfx_dev()->device, buffer->memory, NULL);
    buffer->buffer = VK_NULL_HANDLE;
    buffer->memory = VK_NULL_HANDLE;
}
