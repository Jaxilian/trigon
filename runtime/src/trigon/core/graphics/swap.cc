#include "swap.h"
#include <vulkan/vulkan.h>
#include "vk_swap_supp.h"
#include "gpu.h"
#include "trigon/core/util/err.h"
#include "win.h"
#include <array>

swapchain_t::swapchain_t(win_t& window) :_window(window) {
    create_swapchain();
}

swapchain_t::~swapchain_t() {
    swap_data_t* data = (swap_data_t*)_data;
    delete data;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR swap_present_mode(
    const std::vector<VkPresentModeKHR>& availables) {
    for (const auto& available : availables) {
        if (available == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkFormat find_depth_format() {
    return swap_support_t::find_supported_format(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkExtent2D swap_extent(win_t& window, const VkSurfaceCapabilitiesKHR& capabilities) {
    
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D real_extent = { (uint32_t)window.width, (uint32_t)window.height };
        real_extent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, real_extent.width));
        real_extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, real_extent.height));

        return real_extent;
    }
}

VkFormat depth_format() {
    return swap_support_t::find_supported_format(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void swapchain_t::create_swapchain() {
    VkPhysicalDevice pdev   = (VkPhysicalDevice)gpu_t::get().physical;
    VkDevice device         = (VkDevice)gpu_t::get().device;
    VkSurfaceKHR surface    = (VkSurfaceKHR)_window.surface_ptr();

    if (!_data) _data = new swap_data_t();
    swap_data_t* data       = (swap_data_t*)_data;

    VkSwapchainKHR old_swapchain = data->swapchain;
    data->cleanup();

    swap_support_t support(pdev, surface);

    VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(support.formats);
    VkPresentModeKHR present_mode = swap_present_mode(support.present_modes);
    VkExtent2D extent = swap_extent(_window, support.capabilities);

    uint32_t image_count = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 &&
        image_count > support.capabilities.maxImageCount) {
        image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR inf = {};
    inf.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    inf.surface = surface;

    inf.minImageCount = image_count;
    inf.imageFormat = surface_format.format;
    inf.imageColorSpace = surface_format.colorSpace;
    inf.imageExtent = extent;
    inf.imageArrayLayers = 1;
    inf.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] = { gpu_t::get().que_fam.graphics_family, gpu_t::get().que_fam.present_family };

    if (gpu_t::get().que_fam.graphics_family != gpu_t::get().que_fam.present_family) {
        inf.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        inf.queueFamilyIndexCount = 2;
        inf.pQueueFamilyIndices = queue_family_indices;
    }
    else {
        inf.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        inf.queueFamilyIndexCount = 0;      // Optional
        inf.pQueueFamilyIndices = nullptr;  // Optional
    }

    inf.preTransform = support.capabilities.currentTransform;
    inf.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    inf.presentMode = present_mode;
    inf.clipped = VK_TRUE;

    inf.oldSwapchain = old_swapchain == nullptr ? VK_NULL_HANDLE : old_swapchain;

    VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device, &inf, nullptr, &new_swapchain) != VK_SUCCESS) {
       debug_t::err("failed to create swap chain!\n");
    }
    data->destroy_swapchain();
    delete data;
    data = new swap_data_t();
    data->swapchain = new_swapchain;

    vkGetSwapchainImagesKHR(device, new_swapchain, &image_count, nullptr);
    data->images.resize(image_count);
    vkGetSwapchainImagesKHR(device, new_swapchain, &image_count, data->images.data());

    data->image_format = surface_format.format;
    data->extent = extent;

    data->image_views.resize(data->images.size());
    for (size_t i = 0; i < data->images.size(); i++) {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = data->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = data->image_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &view_info, nullptr, &data->image_views[i]) !=
            VK_SUCCESS) {
            debug_t::err("failed to create texture image view!\n");
        }
    }
    
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = depth_format();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = data->image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.dstSubpass = 0;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
    VkRenderPassCreateInfo renderpass_info = {};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderpass_info.pAttachments = attachments.data();
    renderpass_info.subpassCount = 1;
    renderpass_info.pSubpasses = &subpass;
    renderpass_info.dependencyCount = 1;
    renderpass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderpass_info, nullptr, &data->renderpass) != VK_SUCCESS) {
        debug_t::err("failed to create render pass!");
    }

    data->depth_format = find_depth_format();

    data->depth_images.resize(image_count);
    data->depth_memories.resize(image_count);
    data->image_views.resize(image_count);

    for (int i = 0; i < data->images.size(); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = data->depth_format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        device.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            data->depth_images[i],
            data->depth_memories[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = data->depth_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &data->depth_views[i]) != VK_SUCCESS) {
            debug_t::err("failed to create texture image view!");
        }
    }

}


void swapchain_t::refresh() {
    create_swapchain();
}

