#include "vkl.h"
#include <memory.h>

void vkl_create_image_with_info(
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
        .memoryTypeIndex = vkl_get_memory_type(device->physical_device, mem_req.memoryTypeBits, properties)
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


void vkl_image_new(vkl_image_info_t* info, vkl_image_t* out) {
    if (!out || out->initialized) {
        vkl_error("vkl_image_new, argument \"out\" was NULL or already initialized!\n", ERROR_FATAL);
    }

    if (info->mip_levels <= 0) {
        vkl_error("mip_levels was 0! set it to at least 1!\n", ERROR_FATAL);
    }

    VkImageCreateInfo imginf = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = info->type,

        .extent = {
            .width = info->extent.width,
            .height = info->extent.height,
            .depth = info->extent.depth
        },

        .mipLevels = info->mip_levels,
        .arrayLayers = 1,
        .format = info->format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT
    };

    if (vkCreateImage(info->device->device, &imginf, NULL, &out->image) != VK_SUCCESS) {
        vkl_error("failed to create vkl_image!\n", ERROR_FATAL);
    }

    VkMemoryRequirements memreq;
    vkGetImageMemoryRequirements(info->device->device, out->image, &memreq);

    VkMemoryAllocateInfo allocinf = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memreq.size,
        .memoryTypeIndex = vkl_get_memory_type(info->device->physical_device, memreq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    if (vkAllocateMemory(info->device->device, &allocinf, NULL, &out->memory) != VK_SUCCESS) {
        vkl_error("failed to create vkl_image memory!\n", ERROR_FATAL);
    }

    vkBindImageMemory(info->device->device, out->image, out->memory, 0);
    out->initialized = true;

    if (!info->use_sampler) return;

    VkSamplerCreateInfo sampinf = {
        .sType              = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter          = VK_FILTER_LINEAR, // Magnification filter
        .minFilter          = VK_FILTER_LINEAR, // Minification filter
        .addressModeU       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, // U coordinate
        .addressModeV       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, // V coordinate
        .addressModeW       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, // W coordinate
        .anisotropyEnable   = VK_FALSE, // Anisotropy
        .maxAnisotropy      = 1.0f, // Max anisotropy level
        .borderColor        = VK_BORDER_COLOR_INT_OPAQUE_BLACK, // Border color
        .compareEnable      = VK_FALSE, // Comparison mode
        .compareOp          = VK_COMPARE_OP_ALWAYS,
        .mipmapMode         = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .mipLodBias         = 0.0f, // Level-of-detail bias
        .minLod             = 0.0f, // Minimum LOD
        .maxLod             = (float)info->mip_levels, // Maximum LOD

        .unnormalizedCoordinates = VK_FALSE // Coordinate normalization

    };

    if (vkCreateSampler(info->device->device, &sampinf, NULL, &out->sampler) != VK_SUCCESS) {
        vkl_error("failed to create image sampler!\n", ERROR_FATAL);
    }

    VkImageViewCreateInfo viewinf = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = out->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = info->format,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1
    };

    if (vkCreateImageView(info->device->device, &viewinf, NULL, &out->view) != VK_SUCCESS) {
        vkl_error("failed to create image view!\n", ERROR_FATAL);
    }

}

void vkl_image_set(vkl_image_t* image, vkl_buffer_t* buffer) {
/*
    void* data = vkl_buffer_set_advanced(buffer);
    vkl_state_single_cmd();
    vkl_buffer_submit_advanced(buffer, data);
*/
}

void vkl_image_del(vkl_device_t* device, vkl_image_t* in) {
    if (in->sampler != NULL) {
        vkDestroySampler(device->device, in->sampler, NULL);
        vkDestroyImageView(device->device, in->view, NULL);
    }
    vkDestroyImage(device->device, in->image, NULL);
    vkFreeMemory(device->device, in->memory, NULL);
    memset(in, 0, sizeof(vkl_image_t));
}