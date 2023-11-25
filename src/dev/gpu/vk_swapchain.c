#include "vk.h"

static VkSurfaceFormatKHR fetch_surface_format(VkSurfaceFormatKHR* available, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (available[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            available[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available[i];
        }
    }
    return available[0];
}

static VkPresentModeKHR fetch_present_mode(VkPresentModeKHR* available, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (available[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available[i];
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D fetch_swap_extent() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->device.gpu, ctx->win.surface, &capabilities);
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D extents = { ctx->win.width, ctx->win.height };
        extents.width = (capabilities.minImageExtent.width > extents.width) ? capabilities.minImageExtent.width :
            ((capabilities.maxImageExtent.width < extents.width) ? capabilities.maxImageExtent.width : extents.width);

        extents.height = (capabilities.minImageExtent.height > extents.height) ? capabilities.minImageExtent.height :
            ((capabilities.maxImageExtent.height < extents.height) ? capabilities.maxImageExtent.height : extents.height);

        return extents;
    }
}

static uint32_t fetch_image_count() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->device.gpu, ctx->win.surface, &capabilities);

    uint32_t image_count = 0;
    image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 &&
        image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }
    return image_count;
}

static VkFormat fetch_depth_format() {
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
    };

    return vk_format_supported(
        formats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static void create_swapchain() {
    swap_support_t* support = vk_open_swap_support(ctx->device.gpu);

    ctx->swapchain.extent = fetch_swap_extent();
    ctx->swapchain.image_format = fetch_surface_format(support->formats, support->format_count).format;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->device.gpu, ctx->win.surface, &capabilities);

    uint32_t queue_families[] = { ctx->device.graphics_family, ctx->device.present_family };
    VkSwapchainCreateInfoKHR c_info = { 0 };
    c_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    c_info.surface = ctx->win.surface;
    c_info.minImageCount = fetch_image_count();
    c_info.imageFormat = ctx->swapchain.image_format;
    c_info.imageColorSpace = fetch_surface_format(support->formats, support->format_count).colorSpace;
    c_info.imageExtent = ctx->swapchain.extent;
    c_info.imageArrayLayers = 1;
    c_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queue_families[0] != queue_families[1]) {
        c_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        c_info.queueFamilyIndexCount = 2;
        c_info.pQueueFamilyIndices = queue_families;
    }
    else {
        c_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        c_info.queueFamilyIndexCount = 0;      // Optional
        c_info.pQueueFamilyIndices = NULL;  // Optional
    }

    c_info.preTransform = capabilities.currentTransform;
    c_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    c_info.presentMode = fetch_present_mode(support->present_modes, support->present_count);
    c_info.clipped = VK_TRUE;

    c_info.oldSwapchain = ctx->swapchain.old_swap;

    bool success = (vkCreateSwapchainKHR(ctx->device.device, &c_info, NULL, &ctx->swapchain.new_swap) == VK_SUCCESS);
    validate(success, "failed to create Vulkan surface!\n");

    if (ctx->swapchain.old_swap != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(ctx->device.device, ctx->swapchain.old_swap, NULL);
        ctx->swapchain.old_swap = VK_NULL_HANDLE;
    }
    ctx->swapchain.old_swap = ctx->swapchain.new_swap;

    if (ctx->swapchain.images) {
        mem_free(ctx->swapchain.images);
        ctx->swapchain.images = VK_NULL_HANDLE;
    }

    vkGetSwapchainImagesKHR(ctx->device.device, ctx->swapchain.new_swap, &ctx->swapchain.image_count, NULL);
    ctx->swapchain.images = mem_alloc(sizeof(VkImage) * ctx->swapchain.image_count);
    vkGetSwapchainImagesKHR(ctx->device.device, ctx->swapchain.new_swap, &ctx->swapchain.image_count, ctx->swapchain.images);

    vk_close_swap_support(support);
}

static void create_image_views() {
    if (ctx->swapchain.image_views) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyImageView(ctx->device.device, ctx->swapchain.image_views[i], NULL);
        }
        mem_free(ctx->swapchain.image_views);
        ctx->swapchain.image_views = VK_NULL_HANDLE;
    }

    ctx->swapchain.image_views = mem_alloc(sizeof(VkImageView) * ctx->swapchain.image_count);
    validate(ctx->swapchain.image_views, "failed to allocate image views in swapchain!\n");

    for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
        VkImageViewCreateInfo view_inf = { 0 };
        view_inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_inf.image = ctx->swapchain.images[i];
        view_inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_inf.format = ctx->swapchain.image_format;
        view_inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_inf.subresourceRange.baseMipLevel = 0;
        view_inf.subresourceRange.levelCount = 1;
        view_inf.subresourceRange.baseArrayLayer = 0;
        view_inf.subresourceRange.layerCount = 1;

        bool success = false;
        success = vkCreateImageView(ctx->device.device, &view_inf, NULL, &ctx->swapchain.image_views[i]) == VK_SUCCESS;
        validate(success, "failed to create image view in swapchain!\n");
    }
}

static void create_renderpass() {
    if (ctx->swapchain.renderpass) {
        vkDestroyRenderPass(ctx->device.device, ctx->swapchain.renderpass, NULL);
        ctx->swapchain.renderpass = VK_NULL_HANDLE;
    }

    VkAttachmentDescription depthAttachment = { 0 };
    depthAttachment.format = fetch_depth_format();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = { 0 };
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = { 0 };
    colorAttachment.format = ctx->swapchain.image_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = { 0 };
    dependency.dstSubpass = 0;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = { 0 };
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    bool success = false;
    success = vkCreateRenderPass(ctx->device.device, &renderPassInfo, NULL, &ctx->swapchain.renderpass) == VK_SUCCESS;
    validate(success, "failed to create render pass!");

}

static void create_depth_resource() {
    if (ctx->swapchain.depth_images) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyImageView(ctx->device.device, ctx->swapchain.depth_views[i], NULL);
            vkDestroyImage(ctx->device.device, ctx->swapchain.depth_images[i], NULL);
            vkFreeMemory(ctx->device.device, ctx->swapchain.depth_memories[i], NULL);

        }
        mem_free(ctx->swapchain.depth_memories);
        mem_free(ctx->swapchain.depth_views);
        mem_free(ctx->swapchain.depth_images);

        ctx->swapchain.depth_images = VK_NULL_HANDLE;
        ctx->swapchain.depth_views = VK_NULL_HANDLE;
        ctx->swapchain.depth_memories = VK_NULL_HANDLE;
    }

    ctx->swapchain.depth_format = fetch_depth_format();
    ctx->swapchain.depth_images = mem_alloc(sizeof(VkImage) * ctx->swapchain.image_count);
    ctx->swapchain.depth_memories = mem_alloc(sizeof(VkDeviceMemory) * ctx->swapchain.image_count);
    ctx->swapchain.depth_views = mem_alloc(sizeof(VkImageView) * ctx->swapchain.image_count);

    validate(ctx->swapchain.depth_images, "failed to allocate depth images in swapchain!\n");
    validate(ctx->swapchain.depth_memories, "failed to allocate depth device memory in swapchain\n");
    validate(ctx->swapchain.depth_views, "failed to allocate depth view in swapchain\n");

    for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
        VkImageCreateInfo imageInfo = { 0 };
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = ctx->swapchain.extent.width;
        imageInfo.extent.height = ctx->swapchain.extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = ctx->swapchain.depth_format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        vk_create_image_with_info(
            &imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &ctx->swapchain.depth_images[i],
            &ctx->swapchain.depth_memories[i]);

        VkImageViewCreateInfo viewInfo = { 0 };
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = ctx->swapchain.depth_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = ctx->swapchain.depth_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        bool success = false;
        success = vkCreateImageView(ctx->device.device, &viewInfo, NULL, &ctx->swapchain.depth_views[i]) == VK_SUCCESS;
        validate(success, "failed to create texture image view!");

    }

}

static void create_framebuffers() {
    if (ctx->swapchain.framebuffers) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyFramebuffer(ctx->device.device, ctx->swapchain.framebuffers[i], NULL);
        }
        mem_free(ctx->swapchain.framebuffers);
        ctx->swapchain.framebuffers = VK_NULL_HANDLE;
    }

    ctx->swapchain.framebuffers = mem_alloc(sizeof(VkFramebuffer) * ctx->swapchain.image_count);
    validate(ctx->swapchain.framebuffers, "failed to allocate frambuffers in swapchain!\n");

    for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
        VkImageView attachments[] = { ctx->swapchain.image_views[i], ctx->swapchain.depth_views[i] };

        VkFramebufferCreateInfo framebufferInfo = { 0 };
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = ctx->swapchain.renderpass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = ctx->swapchain.extent.width;
        framebufferInfo.height = ctx->swapchain.extent.height;
        framebufferInfo.layers = 1;
        bool success = vkCreateFramebuffer(
            ctx->device.device,
            &framebufferInfo,
            NULL,
            &ctx->swapchain.framebuffers[i]) == VK_SUCCESS;
        validate(success, "failed to create framebuffer in swapchain!");
    }
}

static void create_sync_objects() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(ctx->device.device, ctx->swapchain.finished_semaphores[i], NULL);
        vkDestroySemaphore(ctx->device.device, ctx->swapchain.available_semaphores[i], NULL);
        vkDestroyFence(ctx->device.device, ctx->swapchain.in_flight_fences[i], NULL);
        ctx->swapchain.finished_semaphores[i] = VK_NULL_HANDLE;
        ctx->swapchain.available_semaphores[i] = VK_NULL_HANDLE;
        ctx->swapchain.in_flight_fences[i] = VK_NULL_HANDLE;
    }

    if (!ctx->swapchain.images_in_flight) {
        ctx->swapchain.images_in_flight = mem_alloc(sizeof(VkFence) * ctx->swapchain.image_count);
        validate(ctx->swapchain.images_in_flight, "failed to allocate images in flight!\n");
    }

    for (size_t i = 0; i < ctx->swapchain.image_count; ++i) {
        ctx->swapchain.images_in_flight[i] = VK_NULL_HANDLE;
    }

    VkSemaphoreCreateInfo semaphoreInfo = { 0 };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = { 0 };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        bool success = vkCreateSemaphore(ctx->device.device, &semaphoreInfo, NULL, &ctx->swapchain.available_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateSemaphore(ctx->device.device, &semaphoreInfo, NULL, &ctx->swapchain.finished_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateFence(ctx->device.device, &fenceInfo, NULL, &ctx->swapchain.in_flight_fences[i]) == VK_SUCCESS;

        validate(success, "failed to create synchronization objects for a frame!");
    }
}

void vk_swapchain_new() {
    while (ctx->win.width == 0 || ctx->win.height == 0) {
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(ctx->device.device);
    create_swapchain();
    create_image_views();
    create_renderpass();
    create_depth_resource();
    create_framebuffers();
    create_sync_objects();
    //vk_pipeline_rebuild();
}

void vk_swapchain_del() {
    if (ctx->swapchain.image_views) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyImageView(ctx->device.device, ctx->swapchain.image_views[i], NULL);
        }
        mem_free(ctx->swapchain.image_views);
        ctx->swapchain.image_views = VK_NULL_HANDLE;
    }

    if (ctx->swapchain.renderpass) {
        vkDestroyRenderPass(ctx->device.device, ctx->swapchain.renderpass, NULL);
        ctx->swapchain.renderpass = VK_NULL_HANDLE;
    }

    if (ctx->swapchain.depth_images) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyImageView(ctx->device.device, ctx->swapchain.depth_views[i], NULL);
            vkDestroyImage(ctx->device.device, ctx->swapchain.depth_images[i], NULL);
            vkFreeMemory(ctx->device.device, ctx->swapchain.depth_memories[i], NULL);

        }
        mem_free(ctx->swapchain.depth_memories);
        mem_free(ctx->swapchain.depth_views);
        mem_free(ctx->swapchain.depth_images);
    }

    if (ctx->swapchain.framebuffers) {
        for (uint32_t i = 0; i < ctx->swapchain.image_count; i++) {
            vkDestroyFramebuffer(ctx->device.device, ctx->swapchain.framebuffers[i], NULL);
        }
        mem_free(ctx->swapchain.framebuffers);
        ctx->swapchain.framebuffers = VK_NULL_HANDLE;
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(ctx->device.device, ctx->swapchain.finished_semaphores[i], NULL);
        vkDestroySemaphore(ctx->device.device, ctx->swapchain.available_semaphores[i], NULL);
        vkDestroyFence(ctx->device.device, ctx->swapchain.in_flight_fences[i], NULL);
    }

    if (ctx->swapchain.images_in_flight) {
        for (size_t i = 0; i < ctx->swapchain.image_count; i++) {
            if (ctx->swapchain.images_in_flight[i] != VK_NULL_HANDLE) {
                //vkDestroyFence(context->device.device, context->swapchain.images_in_flight[i], NULL);
            }
        }
        mem_free(ctx->swapchain.images_in_flight);
    }

    if (ctx->swapchain.images) {
        mem_free(ctx->swapchain.images);
    }

    if (ctx->swapchain.new_swap) {
        vkDestroySwapchainKHR(ctx->device.device, ctx->swapchain.new_swap, NULL);
    }
}

