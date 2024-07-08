#include "vkdef.h"
#include "trigon/core/utils/debug.h"

uint32_t vk_memory_type(VkPhysicalDevice device, uint32_t type, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties mem_prop;
	vkGetPhysicalDeviceMemoryProperties(device, &mem_prop);
	for (uint32_t i = 0; i < mem_prop.memoryTypeCount; i++) {
		if ((type & (1 << i)) && (mem_prop.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	debug_err("failed to find suitable memory type!");
	return 0;
};