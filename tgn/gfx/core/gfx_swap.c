#include "gfxctx.h"
#include "cmn/cmn.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

static VkFormat find_depth_format(VkPhysicalDevice physicalDevice) {
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, formats[i], &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return formats[i];
        }
    }

    debug_err("Failed to find a supported depth format!\n");
    return VK_FORMAT_UNDEFINED;
}

uint32_t gfx_find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    debug_err("Failed to find suitable memory type!\n");
    return UINT32_MAX;
}

void gfx_swap_cleanup(win_t* win) {
    if (!win->device->device) return;

    vkDeviceWaitIdle(win->device->device);

    // Destroy depth resources
    if (win->swapchain.depth_views) {
        for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
            vkDestroyImageView(win->device->device, win->swapchain.depth_views[i], NULL);
        }
        free(win->swapchain.depth_views);
        win->swapchain.depth_views = NULL;
    }

    if (win->swapchain.depth_images) {
        for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
            vkDestroyImage(win->device->device, win->swapchain.depth_images[i], NULL);
        }
        free(win->swapchain.depth_images);
        win->swapchain.depth_images = NULL;
    }

    if (win->swapchain.depth_memories) {
        for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
            vkFreeMemory(win->device->device, win->swapchain.depth_memories[i], NULL);
        }
        free(win->swapchain.depth_memories);
        win->swapchain.depth_memories = NULL;
    }

    // Destroy image views
    if (win->swapchain.image_views) {
        for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
            vkDestroyImageView(win->device->device, win->swapchain.image_views[i], NULL);
        }
        free(win->swapchain.image_views);
        win->swapchain.image_views = NULL;
    }

    // Destroy framebuffers
    if (win->swapchain.framebuffers) {
        for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
            vkDestroyFramebuffer(win->device->device, win->swapchain.framebuffers[i], NULL);
        }
        free(win->swapchain.framebuffers);
        win->swapchain.framebuffers = NULL;
    }

    // Destroy render pass
    if (win->swapchain.renderpass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(win->device->device, win->swapchain.renderpass, NULL);
        win->swapchain.renderpass = VK_NULL_HANDLE;
    }

    // Destroy swapchain
    if (win->swapchain.new_swap != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(win->device->device, win->swapchain.new_swap, NULL);
        win->swapchain.new_swap = VK_NULL_HANDLE;
    }

    // Note: Do not destroy synchronization objects here
}

void gfx_swap_sync(win_t* win) {
    if (win->width == 0 || win->height == 0 || !win->device) return;

    vkDeviceWaitIdle(win->device->device);

    gfx_swap_cleanup(win);

    // Get surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        win->device->physical,
        win->surface,
        &capabilities
    );

    // Choose swap extent
    VkExtent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    }
    else {
        extent.width = win->width;
        extent.height = win->height;

        if (extent.width < capabilities.minImageExtent.width)
            extent.width = capabilities.minImageExtent.width;
        else if (extent.width > capabilities.maxImageExtent.width)
            extent.width = capabilities.maxImageExtent.width;

        if (extent.height < capabilities.minImageExtent.height)
            extent.height = capabilities.minImageExtent.height;
        else if (extent.height > capabilities.maxImageExtent.height)
            extent.height = capabilities.maxImageExtent.height;
    }

    win->swapchain.extent = extent;

    // Choose surface format
    VkSurfaceFormatKHR surfaceFormat = win->device->available_formats[0];
    for (uint32_t i = 0; i < win->device->available_formats_count; i++) {
        if (win->device->available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            win->device->available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = win->device->available_formats[i];
            break;
        }
    }

    win->swapchain.image_format = surfaceFormat.format;

    // Choose present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < win->device->available_present_modes_count; i++) {
        if (win->device->available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        else if (win->device->available_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine number of images
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    win->swapchain.image_count = imageCount;

    // Create swapchain
    VkSwapchainKHR oldSwapchain = win->swapchain.new_swap;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = { 0 };
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = win->surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = win->swapchain.image_format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = win->swapchain.extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { win->device->graphics_family, win->device->present_family };

    if (win->device->graphics_family != win->device->present_family) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;

    if (vkCreateSwapchainKHR(win->device->device, &swapchainCreateInfo, NULL, &win->swapchain.new_swap) != VK_SUCCESS) {
        debug_err("Failed to create Vulkan swapchain!\n");
        return;
    }

    // Destroy old swapchain
    if (oldSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(win->device->device, oldSwapchain, NULL);
    }

    // Get swapchain images
    vkGetSwapchainImagesKHR(win->device->device, win->swapchain.new_swap, &win->swapchain.image_count, NULL);

    win->swapchain.images = calloc(win->swapchain.image_count, sizeof(VkImage));
    if (!win->swapchain.images) {
        debug_err("Failed to allocate memory for swapchain images\n");
        return;
    }

    vkGetSwapchainImagesKHR(win->device->device, win->swapchain.new_swap, &win->swapchain.image_count, win->swapchain.images);

    // Create image views
    win->swapchain.image_views = malloc(sizeof(VkImageView) * win->swapchain.image_count);
    if (!win->swapchain.image_views) {
        debug_err("Failed to allocate memory for swapchain image views!\n");
        return;
    }

    for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
        VkImageViewCreateInfo viewInfo = { 0 };
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = win->swapchain.images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = win->swapchain.image_format;
        viewInfo.components = (VkComponentMapping){
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(win->device->device, &viewInfo, NULL, &win->swapchain.image_views[i]) != VK_SUCCESS) {
            debug_err("Failed to create image views for swapchain\n");
            return;
        }
    }

    win->swapchain.depth_format = find_depth_format(win->device->physical);
    if (win->swapchain.depth_format == VK_FORMAT_UNDEFINED) {
        debug_err("Failed to find suitable depth format!\n");
        return;
    }

    // Allocate arrays for depth images and views
    win->swapchain.depth_images = malloc(sizeof(VkImage) * win->swapchain.image_count);
    win->swapchain.depth_memories = malloc(sizeof(VkDeviceMemory) * win->swapchain.image_count);
    win->swapchain.depth_views = malloc(sizeof(VkImageView) * win->swapchain.image_count);

    if (!win->swapchain.depth_images || !win->swapchain.depth_memories || !win->swapchain.depth_views) {
        debug_err("Failed to allocate memory for depth resources!\n");
        return;
    }

    for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
        // Create depth image
        VkImageCreateInfo image_info = { 0 };
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.format = win->swapchain.depth_format;
        image_info.extent = (VkExtent3D){
            win->swapchain.extent.width,
            win->swapchain.extent.height,
            1
        };
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (vkCreateImage(win->device->device, &image_info, NULL, &win->swapchain.depth_images[i]) != VK_SUCCESS) {
            debug_err("Failed to create depth image!\n");
            return;
        }

        // Allocate memory for depth image
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(win->device->device, win->swapchain.depth_images[i], &mem_requirements);

        VkMemoryAllocateInfo alloc_info = { 0 };
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = gfx_find_memory_type(
            win->device->physical,
            mem_requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        if (alloc_info.memoryTypeIndex == UINT32_MAX) {
            debug_err("Failed to find memory type for depth image!\n");
            return;
        }

        if (vkAllocateMemory(win->device->device, &alloc_info, NULL, &win->swapchain.depth_memories[i]) != VK_SUCCESS) {
            debug_err("Failed to allocate depth image memory!\n");
            return;
        }

        vkBindImageMemory(win->device->device, win->swapchain.depth_images[i], win->swapchain.depth_memories[i], 0);

        // Create depth image view
        VkImageViewCreateInfo view_info = { 0 };
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = win->swapchain.depth_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = win->swapchain.depth_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(win->device->device, &view_info, NULL, &win->swapchain.depth_views[i]) != VK_SUCCESS) {
            debug_err("Failed to create depth image view!\n");
            return;
        }
    }

    // Create render pass
    VkAttachmentDescription colorAttachment = { 0 };
    colorAttachment.format = win->swapchain.image_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = { 0 };
    depthAttachment.format = win->swapchain.depth_format;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = { 0 };
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = { 0 };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = { 0 };
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(win->device->device, &renderPassInfo, NULL, &win->swapchain.renderpass) != VK_SUCCESS) {
        debug_err("Failed to create render pass\n");
        return;
    }

    // Create framebuffers
    win->swapchain.framebuffers = malloc(sizeof(VkFramebuffer) * win->swapchain.image_count);
    if (!win->swapchain.framebuffers) {
        debug_err("Failed to allocate memory for swapchain framebuffers!\n");
        return;
    }

    for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
        VkImageView attachments[] = {
            win->swapchain.image_views[i],
            win->swapchain.depth_views[i]
        };

        VkFramebufferCreateInfo framebufferInfo = { 0 };
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = win->swapchain.renderpass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = win->swapchain.extent.width;
        framebufferInfo.height = win->swapchain.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(win->device->device, &framebufferInfo, NULL, &win->swapchain.framebuffers[i]) != VK_SUCCESS) {
            debug_err("Failed to create framebuffers for swapchain\n");
            return;
        }
    }

    // Create images in flight
    if (win->swapchain.images_in_flight) {
        free(win->swapchain.images_in_flight);
        win->swapchain.images_in_flight = NULL;
    }

    win->swapchain.images_in_flight = malloc(sizeof(VkFence) * win->swapchain.image_count);
    if (!win->swapchain.images_in_flight) {
        debug_err("Failed to allocate images_in_flight array\n");
        return;
    }

    for (uint32_t i = 0; i < win->swapchain.image_count; i++) {
        win->swapchain.images_in_flight[i] = VK_NULL_HANDLE;
    }
}

void gfx_sync_objects_create(win_t* win) {
    VkSemaphoreCreateInfo semaphoreInfo = { 0 };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = { 0 };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(win->device->device, &semaphoreInfo, NULL, &win->swapchain.available_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(win->device->device, &semaphoreInfo, NULL, &win->swapchain.finished_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(win->device->device, &fenceInfo, NULL, &win->swapchain.in_flight_fences[i]) != VK_SUCCESS) {
            debug_err("Failed to create synchronization objects for a frame!\n");
            return;
        }
    }
}

void gfx_sync_objects_destroy(win_t* win) {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (win->swapchain.available_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(win->device->device, win->swapchain.available_semaphores[i], NULL);
            win->swapchain.available_semaphores[i] = VK_NULL_HANDLE;
        }
        if (win->swapchain.finished_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(win->device->device, win->swapchain.finished_semaphores[i], NULL);
            win->swapchain.finished_semaphores[i] = VK_NULL_HANDLE;
        }
        if (win->swapchain.in_flight_fences[i] != VK_NULL_HANDLE) {
            vkDestroyFence(win->device->device, win->swapchain.in_flight_fences[i], NULL);
            win->swapchain.in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }
}

void gfx_swap_del(win_t* win) {
    if (!win->device->device) return;

    vkDeviceWaitIdle(win->device->device);

    gfx_swap_cleanup(win);

    gfx_sync_objects_destroy(win);

    // Free images in flight
    if (win->swapchain.images_in_flight) {
        free(win->swapchain.images_in_flight);
        win->swapchain.images_in_flight = NULL;
    }

    // Free swapchain images
    if (win->swapchain.images) {
        free(win->swapchain.images);
        win->swapchain.images = NULL;
    }
}
