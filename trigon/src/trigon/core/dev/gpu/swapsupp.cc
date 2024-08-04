#include "gpu.h"
#include "trigon/core/dev/win/window.h"
#include <stdlib.h>

swapsupp_t::swapsupp_t(VkPhysicalDevice& gpu) {

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        gpu,
        window_t::main().surface,
        &capabilities
    );

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        gpu,
        window_t::main().surface,
        &format_count,
        NULL
    );

    if (format_count != 0) { 
        formats = (VkSurfaceFormatKHR*) malloc(
            sizeof(VkSurfaceFormatKHR) * format_count
        );

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            gpu,
            window_t::main().surface,
            &format_count, 
            formats
        );
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        gpu,
        window_t::main().surface,
        &present_count,
        NULL
    );

    if (present_count != 0) {

        presents = (VkPresentModeKHR*)malloc(
            sizeof(VkPresentModeKHR) * present_count
        );

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            gpu,
            window_t::main().surface,
            &present_count,
            presents
        );
    }
}

swapsupp_t::~swapsupp_t() {
    free(presents);
    free(formats);
}

