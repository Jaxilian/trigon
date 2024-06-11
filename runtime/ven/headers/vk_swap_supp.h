#pragma once
#include <vulkan/vulkan.h>
#include "trigon/core/graphics/gpu.h"
#include "trigon/core/util/err.h"
#include <vector>
#define MAX_FRAMES_IN_FLIGHT 2

struct swap_data_t {
	VkSwapchainKHR				swapchain = VK_NULL_HANDLE;
	std::vector<VkImage>		images;
	std::vector<VkImageView>	image_views;
	uint32_t					image_count;
	VkFormat					image_format;
	VkExtent2D					extent;
	VkRenderPass				renderpass;
	VkFormat					depth_format;
	std::vector<VkImage>		depth_images;
	std::vector<VkDeviceMemory> depth_memories;
	std::vector<VkImageView>	depth_views;
	std::vector<VkFramebuffer>	framebuffers;
	VkSemaphore					available_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore					finished_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence						in_flight_fences[MAX_FRAMES_IN_FLIGHT];
	std::vector<VkFence>		images_in_flight;
	uint32_t					current_frame;

	void cleanup() {
		VkDevice device = (VkDevice)gpu_t::get().device;
		vkDeviceWaitIdle(device);

		for (auto imageView : image_views) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		image_views.clear();

		for (int i = 0; i < depth_images.size(); i++) {
			vkDestroyImageView(device, image_views[i], nullptr);
			vkDestroyImage(device, depth_images[i], nullptr);
			vkFreeMemory(device, depth_memories[i], nullptr);
		}

		for (auto framebuffer : framebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyRenderPass(device, renderpass, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, finished_semaphores[i], nullptr);
			vkDestroySemaphore(device, available_semaphores[i], nullptr);
			vkDestroyFence(device, in_flight_fences[i], nullptr);
		}
	}

	void destroy_swapchain() {
		VkDevice device = (VkDevice)gpu_t::get().device;
		vkDeviceWaitIdle(device);

		if (swapchain != nullptr) {
			vkDestroySwapchainKHR(device, swapchain, nullptr);
			swapchain = nullptr;
		}
	}

	~swap_data_t() {
		VkDevice device = (VkDevice)gpu_t::get().device;
		vkDeviceWaitIdle(device);

		for (auto imageView : image_views) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		image_views.clear();

		if (swapchain != nullptr) {
			vkDestroySwapchainKHR(device, swapchain, nullptr);
			swapchain = nullptr;
		}

		for (int i = 0; i < depth_images.size(); i++) {
			vkDestroyImageView(device, image_views[i], nullptr);
			vkDestroyImage(device, depth_images[i], nullptr);
			vkFreeMemory(device, depth_memories[i], nullptr);
		}

		for (auto framebuffer : framebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyRenderPass(device, renderpass, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, finished_semaphores[i], nullptr);
			vkDestroySemaphore(device, available_semaphores[i], nullptr);
			vkDestroyFence(device, in_flight_fences[i], nullptr);
		}
	}
};

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

	static VkFormat find_supported_format(
		const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties((VkPhysicalDevice)gpu_t::get().physical, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (
				tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		debug_t::err("failed to find supported format!");
	}

};

