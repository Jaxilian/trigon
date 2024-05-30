#pragma once
#include <vulkan/vulkan.h>
#include <vector>

struct swap_support_t {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	present_modes;

	swap_support_t(VkPhysicalDevice device, VkSurfaceKHR surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
        formats = std::vector<VkSurfaceFormatKHR>(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats.data());
        
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);
        present_modes = std::vector<VkPresentModeKHR>(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &present_mode_count,
            present_modes.data());
       
	}
};