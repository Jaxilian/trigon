#include "gfx_types.h"
#include "cmn/cmn.h"
#include <memory.h>

VkResult gfx_image_new(gfx_image_info_t* info, gfx_image_t* out) {

    if (info == NULL || out == NULL) {
        debug_err("Invalid arguments: info or out is NULL.\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = info->width;
    imageInfo.extent.height = info->height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = info->mipmap_count;
    imageInfo.arrayLayers = 1;
    imageInfo.format = info->format;
    imageInfo.tiling = info->tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = info->usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    VkDevice device = gfx_device_get(NULL)->device;
    VkResult result = vkCreateImage(device, &imageInfo, NULL, &out->image);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create image. VkResult: %d\n", result);
        return result;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, out->image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = gfx_find_memory_type(
        gfx_device_get(NULL)->physical,
        memRequirements.memoryTypeBits,
        info->properties
    );

    result = vkAllocateMemory(device, &allocInfo, NULL, &out->memory);
    if (result != VK_SUCCESS) {
        debug_err("Failed to allocate image memory. VkResult: %d\n", result);
        vkDestroyImage(device, out->image, NULL);
        out->image = VK_NULL_HANDLE;
        return result;
    }

    vkBindImageMemory(device, out->image, out->memory, 0);

    // Initialize the output structure
    out->width = info->width;
    out->height = info->height;
    out->format = info->format;
    out->mipmap_count = info->mipmap_count;
    out->view = VK_NULL_HANDLE; // Initialize view to null

    return VK_SUCCESS;
}

VkResult  gfx_image_view_create(gfx_image_t* image, VkImageAspectFlags aspect) {
    if (image == NULL || image->image == VK_NULL_HANDLE) {
        debug_err("Invalid argument: image is NULL or uninitialized.\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image->image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = image->format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = aspect;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = image->mipmap_count;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkDevice device = gfx_device_get(NULL)->device;
    VkResult result = vkCreateImageView(device, &viewInfo, NULL, &image->view);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create image view. VkResult: %d\n", result);
    }

    return result;
}

void gfx_image_transition(VkImageLayout new_layout, VkImageLayout old_layout, gfx_image_t* image) {

    if (image == NULL || image->image == VK_NULL_HANDLE) {
        debug_err("Invalid argument: image is NULL or uninitialized.\n");
        return;
    }

    VkCommandBuffer cmd_buffer = gfx_cmd_begin();
    if (cmd_buffer == VK_NULL_HANDLE) {
        debug_err("Failed to begin command buffer.\n");
        return;
    }

    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image->image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = image->mipmap_count;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        debug_err("Unsupported layout transition from %d to %d.\n", old_layout, new_layout);
        gfx_cmd_end(cmd_buffer);
        return;
    }

    vkCmdPipelineBarrier(
        cmd_buffer,
        source_stage, destination_stage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );
    gfx_cmd_end(cmd_buffer);
}

void gfx_image_insert_data(gfx_image_t* image, VkBuffer data) {
    if (image == NULL || image->image == VK_NULL_HANDLE || data == VK_NULL_HANDLE) {
        debug_err("Invalid arguments: image or data buffer is NULL or uninitialized.\n");
        return;
    }

    VkCommandBuffer cmd_buffer = gfx_cmd_begin();
    if (cmd_buffer == VK_NULL_HANDLE) {
        debug_err("Failed to begin command buffer.\n");
        return;
    }

    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){ 0, 0, 0 };
    region.imageExtent = (VkExtent3D){ image->width, image->height, 1 };

    vkCmdCopyBufferToImage(
        cmd_buffer,
        data,
        image->image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    gfx_cmd_end(cmd_buffer);
}

void gfx_image_del(gfx_image_t* image) {
    if (image == NULL) return;

    VkDevice device = gfx_device_get(NULL)->device;

    if (image->view != VK_NULL_HANDLE) {
        vkDestroyImageView(device, image->view, NULL);
        image->view = VK_NULL_HANDLE;
    }

    if (image->image != VK_NULL_HANDLE) {
        vkDestroyImage(device, image->image, NULL);
        image->image = VK_NULL_HANDLE;
    }

    if (image->memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, image->memory, NULL);
        image->memory = VK_NULL_HANDLE;
    }

    // Reset other fields
    image->width = 0;
    image->height = 0;
    image->mipmap_count = 0;
    image->format = VK_FORMAT_UNDEFINED;
}

void gfx_image_texture(VkFormat format, uint32_t width, uint32_t height, uint32_t channels, uint32_t mipmap, const void* pixelData, gfx_image_t* out) {
    if (pixelData == NULL || out == NULL) {
        debug_err("Invalid arguments: pixelData or out is NULL.\n");
        return;
    }

    // Determine bytes per pixel based on format
    switch (format) {
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_UNORM:
        channels = 4;
        break;
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_B8G8R8_UNORM:
        channels = 3;
        break;
        // Add cases for other formats as needed
    default:
        debug_err("Unsupported image format.\n");
        return;
    }

    VkDeviceSize image_size = width * height * channels;

    gfx_buffer_info_t info = {0};
    info.size = image_size;
    info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    gfx_buffer_t buffer = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    VkResult result = gfx_buffer_new(&info, &buffer);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create staging buffer. VkResult: %d\n", result);
        return;
    }

    void* data;
    result = vkMapMemory(gfx_device_get(NULL)->device, buffer.memory, 0, image_size, 0, &data);
    if (result != VK_SUCCESS) {
        debug_err("Failed to map buffer memory. VkResult: %d\n", result);
        gfx_buffer_del(&buffer);
        return;
    }
    memcpy(data, pixelData, (size_t)image_size);
    vkUnmapMemory(gfx_device_get(NULL)->device, buffer.memory);

    gfx_image_info_t img_info = {0};
    img_info.width = width;
    img_info.height = height;
    img_info.format = format;
    img_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    img_info.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    img_info.mipmap_count = mipmap;
    img_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    result = gfx_image_new(&img_info, out);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create image. VkResult: %d\n", result);
        gfx_buffer_del(&buffer);
        return;
    }

    gfx_image_transition(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, out);
    gfx_image_insert_data(out, buffer.buffer);
    gfx_image_transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, out);

    result = gfx_image_view_create(out, VK_IMAGE_ASPECT_COLOR_BIT);
    if (result != VK_SUCCESS) {
        debug_err("Failed to create image view. VkResult: %d\n", result);
        gfx_image_del(out);
        gfx_buffer_del(&buffer);
        return;
    }

    gfx_buffer_del(&buffer);
}

