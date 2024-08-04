#include "gpu.h"
#include <stdlib.h>

gpu_t::gpu_t() {

    u32 dc = 0;
    vkEnumeratePhysicalDevices(
        vkinst_t::ref().vki,
        &dc,
        nullptr
    );

    debug_log("GPU count: %d\n", dc);
    cassert(dc > 0, "failed to find gpu with vulkan support");

    VkPhysicalDevice* gpus = nullptr;
    gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * dc);

    vkEnumeratePhysicalDevices(
        vkinst_t::ref().vki,
        &dc,
        gpus
    );

    for (u32 i = 0; i < dc; i++) {

        VkPhysicalDevice pd = gpus[i];
        qfi_t qfi(pd);
        swapsupp_t supp(pd);

        b8 ext_supported = extension_supported(
            pd,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        );

        if (!ext_supported) continue;

        b8 swap_ready = supp.format_count > 0 && supp.present_count > 0;
        b8 qfi_valid = qfi.gfam_valued && qfi.pfam_valued;

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(pd, &features);

        b8 suitable = (
            qfi_valid &&
            swap_ready &&
            ext_supported &&
            features.samplerAnisotropy
        );

        if (!suitable) continue;
        handle = pd;
        pfam = qfi.pfam;
        gfam = qfi.gfam;
        break;
    }

    VkPhysicalDeviceProperties props = {};
    vkGetPhysicalDeviceProperties(handle, &props);
    debug_log("GPU: %s\n", props.deviceName);
    free(gpus);
}

gpu_t::~gpu_t() {

}

b8 gpu_t::extension_supported(VkPhysicalDevice pd, cstr_t name) {

    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(
        pd,
        NULL,
        &extc,
        NULL
    );

    VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(
        sizeof(VkExtensionProperties) * extc
    );

    if (extensions == NULL) {
        debug_err("failed to allocate memory for device extensions\n");
    }

    vkEnumerateDeviceExtensionProperties(
        pd,
        NULL,
        &extc,
        extensions
    );

    for (uint32_t i = 0; i < extc; i++) {
        if ( strcmp(
            extensions[i].extensionName,
            name) == 0) {
            free(extensions);
            return true;
        }
    }
    free(extensions);
    return false;
}

u32 gpu_t::get_mem_type(u32 type, VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties props = {};
    vkGetPhysicalDeviceMemoryProperties(handle, &props);

    for (u32 i = 0; i < props.memoryTypeCount; i++) {
        if ( (type & (1 << i) )
            && 
            (props.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    debug_err("failed to find suitable memory type!");
    return 0;
}

VkFormat gpu_t::format_supported(
    const VkFormat* candidates,
    u32 candidate_count,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {
    for (u32 i = 0; i < candidate_count; i++) {
        VkFormatProperties props = {};
        vkGetPhysicalDeviceFormatProperties(handle, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return  candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features) {
            return  candidates[i];
        }
    }

    debug_err("failed to find supported format!\n");
    return (VkFormat)0;
}