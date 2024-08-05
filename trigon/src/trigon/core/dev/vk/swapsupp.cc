#include "vulkan.h"
#include "trigon/core/dev/win/window.h"
#include <stdlib.h>

swapsupp_t::swapsupp_t(VkPhysicalDevice& gpu) {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        gpu,
        vkdev_t::ref().surface,
        &capabilities
    );

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        gpu,
        vkdev_t::ref().surface,
        &format_count,
        NULL
    );

    if (format_count != 0) { 
        formats = (VkSurfaceFormatKHR*) malloc(
            sizeof(VkSurfaceFormatKHR) * format_count
        );

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            gpu,
            vkdev_t::ref().surface,
            &format_count, 
            formats
        );
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        gpu,
        vkdev_t::ref().surface,
        &present_count,
        NULL
    );

    if (present_count != 0) {

        presents = (VkPresentModeKHR*)malloc(
            sizeof(VkPresentModeKHR) * present_count
        );

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            gpu,
            vkdev_t::ref().surface,
            &present_count,
            presents
        );
    }
}

swapsupp_t::~swapsupp_t() {
    free(presents);
    free(formats);
}

