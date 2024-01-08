#include "vkl.h"

VkFormat vkl_format_supported(
    const vkl_device_t* device,
    const VkFormat* candidates,
    uint32_t candidate_count,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {

    for (uint32_t i = 0; i < candidate_count; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return  candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return  candidates[i];
        }
    }

    vkl_error("failed to find supported format!\n", ERROR_FATAL);
    return 0;
}

