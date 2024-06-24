#include "vkdef.h"
#include "trigon/core/utils/debug.h"

#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

static VkFormat format_supported(
    const vkdevice_t* device,
    const VkFormat* candidates,
    uint32_t candidate_count,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {

    for (uint32_t i = 0; i < candidate_count; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->physical, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return  candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return  candidates[i];
        }
    }

    debug_err("failed to find supported format!\n");
    return 0;
}

static uint32_t get_memory_type(VkPhysicalDevice device, uint32_t type, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_prop;
    vkGetPhysicalDeviceMemoryProperties(device, &mem_prop);
    for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++) {
        if ((type & (1 << i)) &&
            (mem_prop.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    debug_err("failed to find suitable memory type!");
    return 0;
}

static void create_depth_image_with_info(
    const vkdevice_t* device,
    const VkImageCreateInfo* info,
    VkMemoryPropertyFlags properties,
    VkImage* image,
    VkDeviceMemory* image_memory)
{
    bool success = vkCreateImage(device->device, info, NULL, image) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to create image!\n");
    }

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(device->device, *image, &mem_req);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = get_memory_type(device->physical, mem_req.memoryTypeBits, properties)
    };

    success = vkAllocateMemory(device->device, &alloc_info, NULL, image_memory) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to allocate image memory!");
    }

    success = vkBindImageMemory(device->device, *image, *image_memory, 0) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to bind image memory!");
    }

}

static VkSurfaceFormatKHR fetch_surface_format(VkSurfaceFormatKHR* available, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (available[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            available[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available[i];
        }
    }
    return available[0];
}

static VkExtent2D fetch_swap_extent(vkdevice_t* device, VkExtent2D extent) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical, device->main_surface, &capabilities);
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D extents = extent;
        extents.width = (capabilities.minImageExtent.width > extents.width) ? capabilities.minImageExtent.width :
            ((capabilities.maxImageExtent.width < extents.width) ? capabilities.maxImageExtent.width : extents.width);

        extents.height = (capabilities.minImageExtent.height > extents.height) ? capabilities.minImageExtent.height :
            ((capabilities.maxImageExtent.height < extents.height) ? capabilities.maxImageExtent.height : extents.height);

        return extents;
    }
}

static uint32_t fetch_image_count(vkdevice_t* device) {

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device->physical,
        device->main_surface,
        &capabilities
    );

    uint32_t image_count = 0;
    image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 &&
        image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }
    return image_count;
}

static VkPresentModeKHR fetch_present_mode(VkPresentModeKHR* available, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (available[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available[i];
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkFormat fetch_depth_format(vkdevice_t* device) {
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
    };

    return format_supported(
        device,
        formats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static void create_swapchain(vkdevice_t* device, vkswapchain_t* swapchain, VkExtent2D extent) {

    swapchain->extent = fetch_swap_extent(device, extent);
    swapchain->image_format = fetch_surface_format(
        device->available_formats,
        device->available_formats_c
    ).format;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device->physical,
        device->main_surface,
        &capabilities
    );

    uint32_t queue_families[] = {
        device->graphics_family,
        device->present_family
    };

    VkSwapchainCreateInfoKHR c_info = { 0 };
    c_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    c_info.surface = device->main_surface;
    c_info.minImageCount = fetch_image_count(device);
    c_info.imageFormat = swapchain->image_format;
    c_info.imageColorSpace = fetch_surface_format(device->available_formats, device->available_formats_c).colorSpace;
    c_info.imageExtent = extent;
    c_info.imageArrayLayers = 1;
    c_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queue_families[0] != queue_families[1]) {
        c_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        c_info.queueFamilyIndexCount = 2;
        c_info.pQueueFamilyIndices = queue_families;
    }
    else {
        c_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        c_info.queueFamilyIndexCount = 0;
        c_info.pQueueFamilyIndices = NULL;
    }

    c_info.preTransform = capabilities.currentTransform;
    c_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    c_info.presentMode = fetch_present_mode(device->available_present_modes, device->available_present_modes_c);
    c_info.clipped = VK_TRUE;

    c_info.oldSwapchain = swapchain->old_swap;

    bool success = (vkCreateSwapchainKHR(device->device, &c_info, NULL, &swapchain->new_swap) == VK_SUCCESS);
    if (!success) {
        debug_err("failed to create vulkan swapchain!\n");
    }

    if (swapchain->old_swap != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device->device, swapchain->old_swap, NULL);
        swapchain->old_swap = VK_NULL_HANDLE;
    }
    swapchain->old_swap = swapchain->new_swap;

    if (swapchain->images) {
        free(swapchain->images);
        swapchain->images = VK_NULL_HANDLE;
    }

    vkGetSwapchainImagesKHR(device->device, swapchain->new_swap, &swapchain->image_count, NULL);
    swapchain->images = malloc(sizeof(VkImage) * swapchain->image_count);
    vkGetSwapchainImagesKHR(device->device, swapchain->new_swap, &swapchain->image_count, swapchain->images);
}

static void create_image_views(vkdevice_t* device, vkswapchain_t* swapchain) {
    if (swapchain->image_views) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyImageView(device->device, swapchain->image_views[i], NULL);
        }
        free(swapchain->image_views);
        swapchain->image_views = VK_NULL_HANDLE;
    }

    swapchain->image_views = malloc(sizeof(VkImageView) * swapchain->image_count);
    if (swapchain->image_views == NULL) {
        debug_err("failed to allocate memory for swapchain image views!\n");
    }

    for (uint32_t i = 0; i < swapchain->image_count; i++) {
        VkImageViewCreateInfo view_inf = { 0 };
        view_inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_inf.image = swapchain->images[i];
        view_inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_inf.format = swapchain->image_format;
        view_inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_inf.subresourceRange.baseMipLevel = 0;
        view_inf.subresourceRange.levelCount = 1;
        view_inf.subresourceRange.baseArrayLayer = 0;
        view_inf.subresourceRange.layerCount = 1;

        bool success = false;
        success = vkCreateImageView(device->device, &view_inf, NULL, &swapchain->image_views[i]) == VK_SUCCESS;
        if (!success) {
            debug_err("failed to create image views for swapchain\n");
        }
    }
}

static void create_renderpass(vkdevice_t* device, vkswapchain_t* swapchain) {
    if (swapchain->renderpass) {
        vkDestroyRenderPass(device->device, swapchain->renderpass, NULL);
        swapchain->renderpass = VK_NULL_HANDLE;
    }

    VkAttachmentDescription depth_attachment = {
        .format = fetch_depth_format(device),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depth_attachment_ref = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription color_attachment = {
        .format = swapchain->image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference color_attachment_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_ref,
        .pDepthStencilAttachment = &depth_attachment_ref
    };

    VkSubpassDependency dependency = {
        .dstSubpass = 0,
        .dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .srcAccessMask = 0,
        .srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
    };

    VkAttachmentDescription attachments[] = {
        color_attachment,
        depth_attachment
    };

    VkRenderPassCreateInfo renderpass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    bool success = false;
    success = vkCreateRenderPass(device->device, &renderpass_info, NULL, &swapchain->renderpass) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to create render pass\n");
    }
}

static void create_depth_resource(vkdevice_t* device, vkswapchain_t* swapchain) {
    if (swapchain->depth_images) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyImageView(device->device, swapchain->depth_views[i], NULL);
            vkDestroyImage(device->device, swapchain->depth_images[i], NULL);
            vkFreeMemory(device->device, swapchain->depth_memories[i], NULL);

        }
        free(swapchain->depth_memories);
        free(swapchain->depth_views);
        free(swapchain->depth_images);

        swapchain->depth_images = VK_NULL_HANDLE;
        swapchain->depth_views = VK_NULL_HANDLE;
        swapchain->depth_memories = VK_NULL_HANDLE;
    }

    swapchain->depth_format = fetch_depth_format(device);
    swapchain->depth_images = malloc(sizeof(VkImage) * swapchain->image_count);
    swapchain->depth_memories = malloc(sizeof(VkDeviceMemory) * swapchain->image_count);
    swapchain->depth_views = malloc(sizeof(VkImageView) * swapchain->image_count);

    if (swapchain->depth_images == NULL) {
        debug_err("failed to allocate memory for swapchain depth images\n");
    }

    if (swapchain->depth_memories == NULL) {
        debug_err("failed to allocate memory for swapchain depth memories\n");
    }

    if (swapchain->depth_views == NULL) {
        debug_err("failed to allocate memory for swapchain depth views\n");
    }

    for (uint32_t i = 0; i < swapchain->image_count; i++) {
        VkImageCreateInfo image_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .extent.width = swapchain->extent.width,
            .extent.height = swapchain->extent.height,
            .extent.depth = 1,
            .mipLevels = 1,
            .arrayLayers = 1,
            .format = swapchain->depth_format,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .flags = 0
        };

        

        create_depth_image_with_info(
            device,
            &image_info,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &swapchain->depth_images[i],
            &swapchain->depth_memories[i]
        );
        

        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->depth_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->depth_format,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        bool success = false;
        success = vkCreateImageView(
            device->device,
            &view_info,
            NULL,
            &swapchain->depth_views[i]
        ) == VK_SUCCESS;

        if (!success) {
            debug_err("failed to create swapchain depth images!\n");
        }

    }

}

static void create_framebuffers(vkdevice_t* device, vkswapchain_t* swapchain) {
    if (swapchain->framebuffers) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyFramebuffer(device->device, swapchain->framebuffers[i], NULL);
        }
        free(swapchain->framebuffers);
        swapchain->framebuffers = VK_NULL_HANDLE;
    }

    swapchain->framebuffers = malloc(sizeof(VkFramebuffer) * swapchain->image_count);
    if (swapchain->framebuffers == NULL) {
        debug_err("failed to allocate memory for swapchain framebuffers\n");
    }

    for (uint32_t i = 0; i < swapchain->image_count; i++) {
        VkImageView attachments[] = { swapchain->image_views[i], swapchain->depth_views[i] };

        VkFramebufferCreateInfo framebuffer_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = swapchain->renderpass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = swapchain->extent.width,
            .height = swapchain->extent.height,
            .layers = 1
        };

        bool success = vkCreateFramebuffer(
            device->device,
            &framebuffer_info,
            NULL,
            &swapchain->framebuffers[i]) == VK_SUCCESS;
        if (!success) {
            debug_err("failed to create swapchain framebuffers\n");
        }
    }
}

static void create_sync_objects(vkdevice_t* device, vkswapchain_t* swapchain) {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device->device, swapchain->finished_semaphores[i], NULL);
        vkDestroySemaphore(device->device, swapchain->available_semaphores[i], NULL);
        vkDestroyFence(device->device, swapchain->in_flight_fences[i], NULL);
        swapchain->finished_semaphores[i] = VK_NULL_HANDLE;
        swapchain->available_semaphores[i] = VK_NULL_HANDLE;
        swapchain->in_flight_fences[i] = VK_NULL_HANDLE;
    }

    if (!swapchain->images_in_flight) {
        swapchain->images_in_flight = malloc(sizeof(VkFence) * swapchain->image_count);
        if (swapchain->images_in_flight == NULL) {
            debug_err("failed to allocate images in flight!\n");
            return;
        }
    }

    for (size_t i = 0; i < swapchain->image_count; ++i) {
        swapchain->images_in_flight[i] = VK_NULL_HANDLE;
    }

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO

    };

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        bool success = vkCreateSemaphore(device->device, &semaphore_info, NULL, &swapchain->available_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateSemaphore(device->device, &semaphore_info, NULL, &swapchain->finished_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateFence(device->device, &fence_info, NULL, &swapchain->in_flight_fences[i]) == VK_SUCCESS;

        if (!success) {
            debug_err("failed to create synchronization objects for a frame!");
        }
    }
}

void swapchain_new(vkdevice_t* device, vkswapchain_t* swapchain, uint32_t extent[2]) {
    if (extent[0] <= 0 || extent[1] <= 0) {
        debug_err("vkl_swapchain_info_t.width/height was smaller or equal to zero! this is not allowed\n");
    }
    vkDeviceWaitIdle(device->device);
    create_swapchain(device, swapchain, (VkExtent2D) { extent[0], extent[1] });
    create_image_views(device, swapchain);
    create_renderpass(device, swapchain);
    create_depth_resource(device, swapchain);
    create_framebuffers(device, swapchain);
    create_sync_objects(device, swapchain);
}

void swapchain_del(vkdevice_t* device, vkswapchain_t* swapchain) {
    if (swapchain->image_views) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyImageView(device->device, swapchain->image_views[i], NULL);
        }
        free(swapchain->image_views);
        swapchain->image_views = VK_NULL_HANDLE;
    }

    if (swapchain->renderpass) {
        vkDestroyRenderPass(device->device, swapchain->renderpass, NULL);
        swapchain->renderpass = VK_NULL_HANDLE;
    }

    if (swapchain->depth_images) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyImageView(device->device, swapchain->depth_views[i], NULL);
            vkDestroyImage(device->device, swapchain->depth_images[i], NULL);
            vkFreeMemory(device->device, swapchain->depth_memories[i], NULL);

        }
        free(swapchain->depth_memories);
        free(swapchain->depth_views);
        free(swapchain->depth_images);
    }

    if (swapchain->framebuffers) {
        for (uint32_t i = 0; i < swapchain->image_count; i++) {
            vkDestroyFramebuffer(device->device, swapchain->framebuffers[i], NULL);
        }
        free(swapchain->framebuffers);
        swapchain->framebuffers = VK_NULL_HANDLE;
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device->device, swapchain->finished_semaphores[i], NULL);
        vkDestroySemaphore(device->device, swapchain->available_semaphores[i], NULL);
        vkDestroyFence(device->device, swapchain->in_flight_fences[i], NULL);
    }

    if (swapchain->images_in_flight) {
        for (size_t i = 0; i < swapchain->image_count; i++) {
            if (swapchain->images_in_flight[i] != VK_NULL_HANDLE) {
                vkDestroyFence(device->device, swapchain->images_in_flight[i], NULL);
            }
        }
        free(swapchain->images_in_flight);
    }

    if (swapchain->images) {
        free(swapchain->images);
    }

    if (swapchain->new_swap) {
        vkDestroySwapchainKHR(device->device, swapchain->new_swap, NULL);
    }
}
