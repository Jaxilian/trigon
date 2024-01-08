#include "vkl.h"

uint32_t vkl_get_memory_type(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_prop;
    vkGetPhysicalDeviceMemoryProperties(device, &mem_prop);
    for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (mem_prop.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    vkl_error("failed to find suitable memory type!", ERROR_WARNING);
    return 0;
}