#include "vk.h"

void vkbuffer_new(
    vkdevice_t* device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer* buffer,
    VkDeviceMemory* memory) {

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(device->device, &buffer_info, NULL, buffer) != VK_SUCCESS) {
        debug_err("failed to create vkbuffer!\n");
    }

    VkMemoryRequirements memreq = { 0 };
    vkGetBufferMemoryRequirements(device->device, *buffer, &memreq);

    VkMemoryAllocateInfo alloc = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memreq.size,
        .memoryTypeIndex = vk_memory_type(device->physical, memreq.memoryTypeBits, properties)
    };

    if (vkAllocateMemory(device->device, &alloc, NULL, memory) != VK_SUCCESS) {
        debug_err("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device->device, *buffer, *memory, 0);
}

void vkbuffer_copy(vkstate_t* state, VkBuffer to, VkBuffer src, VkDeviceSize size) {
    VkCommandBuffer cmdbuff = vkstate_single_cmd(state);
    VkBufferCopy cp = {
        .size = size
    };
    vkCmdCopyBuffer(cmdbuff, src, to, 1, &cp);
    vkstate_single_cmd_submit(state, cmdbuff);
}