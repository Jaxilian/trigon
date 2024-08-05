#include "vulkan.h"
#include <stdlib.h>
#include "trigon/core/dev/win/window.h"

qfi_t::qfi_t(VkPhysicalDevice& gpu) {

    u32 qfic = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qfic, NULL);

    VkQueueFamilyProperties* qf = (VkQueueFamilyProperties*)malloc(
        sizeof(VkQueueFamilyProperties) * qfic
    );

    cassert(qf, "qfi_t failed to allocate memory!\n");

    vkGetPhysicalDeviceQueueFamilyProperties(
        gpu,
        &qfic,
        qf
    );

    for (u32 i = 0; i < qfic; ++i) {
        if (qf[i].queueCount > 0 && qf[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            gfam = i;
            gfam_valued = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            gpu,
            i,
            vkdev_t::ref().surface,
            &present_supported
        );

        if (qf[i].queueCount > 0 && present_supported) {
            pfam = i;
            pfam_valued = true;
        }
        if (pfam_valued && gfam_valued) {
            break;
        }
    }

    free(qf);
}

qfi_t::~qfi_t() {

}