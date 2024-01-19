#include "vkl.h"
#include <stdlib.h>
#include <memory.h>

void vkl_buffer_new(vkl_buffer_info_t* info, vkl_buffer_t* out) {
    if (!out) return;

    out->stride = info->stride;
    out->count = info->count;
    out->size = info->stride * info->count;
    out->initialized = false;
    out->usage_flags = info->usage_flags;
    out->dev_ptr = info->device;

    if (out->size == 0) return;

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = out->size,
        .usage = info->usage_flags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (vkCreateBuffer(info->device->device, &buffer_info, NULL, &out->buffer) != VK_SUCCESS) {
        vkl_error("Failed to create Vulkan buffer!\n", ERROR_WARNING);
    }

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(info->device->device, out->buffer, &mem_req);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vkl_get_memory_type(
            info->device->physical_device,
            mem_req.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        )
    };

    if (vkAllocateMemory(info->device->device, &alloc_info, NULL, &out->memory) != VK_SUCCESS) {
        vkDestroyBuffer(info->device->device, out->buffer, NULL);
        out->buffer = VK_NULL_HANDLE;
        vkl_error("Failed to allocate memory for Vulkan buffer!\n", ERROR_WARNING);
    }

    vkBindBufferMemory(info->device->device, out->buffer, out->memory, 0);
    out->initialized = true;
}

void vkl_buffer_resize(vkl_buffer_t* buffer, uint32_t new_count) {
    if (!buffer || !buffer->initialized) return;

    vkDeviceWaitIdle(buffer->dev_ptr->device);

    VkDeviceSize new_size = buffer->stride * new_count;
    if (new_size == buffer->size) return;

    vkDestroyBuffer(buffer->dev_ptr->device, buffer->buffer, NULL);
    vkFreeMemory(buffer->dev_ptr->device, buffer->memory, NULL);

    buffer->count = new_count;
    buffer->size = new_size;

    vkl_buffer_info_t info = {
        .count = new_count,
        .device = buffer->dev_ptr,
        .stride = buffer->stride,
        .usage_flags = buffer->usage_flags
    };

    vkl_buffer_new(&info, buffer);
}



void* vkl_buffer_set_advanced(vkl_buffer_t* buffer) {
    if (!buffer || !buffer->initialized) return NULL;

    void* data_ptr;
    if (vkMapMemory(buffer->dev_ptr->device, buffer->memory, 0, buffer->size, 0, &data_ptr) != VK_SUCCESS) {
        vkl_error("Failed to map Vulkan buffer memory!\n", ERROR_WARNING);
        return NULL;
    }

    return data_ptr;
}

void vkl_buffer_submit_advanced(vkl_buffer_t* buffer, void* mappedMem) {
    if (!buffer || !buffer->initialized || !mappedMem) return;
   
    VkDeviceSize alligned_size = (buffer->dev_ptr->physical_props.limits.nonCoherentAtomSize - 1) & ~(buffer->dev_ptr->physical_props.limits.nonCoherentAtomSize - 1);

    VkMappedMemoryRange mem_range = {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = buffer->memory,
        .offset = 0,
        .size = alligned_size,
    };

    if (vkFlushMappedMemoryRanges(buffer->dev_ptr->device, 1, &mem_range) != VK_SUCCESS) {
        vkl_error("Failed to flush mapped memory range in Vulkan buffer!\n", ERROR_WARNING);
    }

    vkUnmapMemory(buffer->dev_ptr->device, buffer->memory);
}

void vkl_buffer_set(vkl_buffer_t* buffer, void* data) {
    if (!buffer || !buffer->initialized || !data) return;

    void* mapped_memory;
    if (vkMapMemory(buffer->dev_ptr->device, buffer->memory, 0, buffer->size, 0, &mapped_memory) != VK_SUCCESS) {
        vkl_error("Failed to map Vulkan buffer memory!\n", ERROR_WARNING);
    }

    memcpy(mapped_memory, data, buffer->size);
    vkUnmapMemory(buffer->dev_ptr->device, buffer->memory);
}

void vkl_buffer_del(vkl_buffer_t* buffer) {
    if (!buffer || !buffer->initialized) return;

    vkFreeMemory(buffer->dev_ptr->device, buffer->memory, NULL);
    vkDestroyBuffer(buffer->dev_ptr->device, buffer->buffer, NULL);
    memset(buffer, 0, sizeof(vkl_buffer_t));
}

void vkl_buffer_clear(vkl_buffer_t* arr, uint32_t count) {
    if (!arr) return;

    for (uint32_t i = 0; i < count; i++) {
        vkl_buffer_del(&arr[i]);
    }
}
