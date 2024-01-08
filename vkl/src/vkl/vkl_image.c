#include "vkl.h"

void vk_create_image_with_info(
    const vkl_device_t* device,
    const VkImageCreateInfo* info,
    VkMemoryPropertyFlags properties,
    VkImage* image,
    VkDeviceMemory* image_memory)
{
    bool success = vkCreateImage(device->device, info, NULL, image) == VK_SUCCESS;
    if (!success) {
        vkl_error("failed to create image!\n", ERROR_FATAL);
    }

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(device->device, *image, &mem_req);

    VkMemoryAllocateInfo alloc_info = { 
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vkl_fetch_memory_type(device, mem_req.memoryTypeBits, properties)
    };

    success = vkAllocateMemory(device->device, &alloc_info, NULL, image_memory) == VK_SUCCESS;
    if (!success) {
        vkl_error("failed to allocate image memory!", ERROR_FATAL);
    }

    success = vkBindImageMemory(device->device, *image, *image_memory, 0) == VK_SUCCESS;
    if (!success) {
        vkl_error("failed to bind image memory!", ERROR_FATAL);
    }
}
