#include "vk.h"

void vk_image_new(vk_image_t* image, file_img_t* file) {
    VkDeviceSize image_size = file->width * file->height * file->channels; 
    validate(file->data != NULL, "data was null!");

    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,  // Make sure to match the loaded texture format
        .extent = { file->width,  file->height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    vkCreateImage(
        ctx->device.device,
        &image_info,
        NULL,
        &image->image
    );

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(
        ctx->device.device,
        image->image,
        &mem_req
    );

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = vk_fetch_memory_type(
            mem_req.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        ),
    };

    vkAllocateMemory(ctx->device.device, &alloc_info, NULL, &image->memory);
    vkBindImageMemory(ctx->device.device, image->image, image->memory, 0);

    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image_info.format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCreateImageView(ctx->device.device, &view_info, NULL, &image->view);

    vk_buffer_t buffer = { 0 };
    vk_buffer_new(
        image_size,
        1,
        &buffer,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    );

    vk_buffer_set(&buffer, file->data);
    vk_buffer_del(&buffer);

    image->initialized = true;
}

void vk_image_del() {

}