#include "gfxctx.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmn/cmn.h"

#define VK_CHECK(call) if ((call) != VK_SUCCESS) { fprintf(stderr, "Vulkan Error at %s:%d\n", __FILE__, __LINE__); exit(-1); }

static vkdev_t device = { 0 };

static bool get_queue_families(vkdev_t* device, VkSurfaceKHR surface, uint32_t* graphics_family, uint32_t* present_family) {
	uint32_t queue_fam_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_fam_count, NULL);

	VkQueueFamilyProperties* queue_families = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queue_fam_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_fam_count, queue_families);

	bool found_graphics = false, found_present = false;
	for (uint32_t i = 0; i < queue_fam_count; ++i) {
		if (queue_families[i].queueCount > 0 && (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			*graphics_family = i;
			found_graphics = true;
		}

		VkBool32 present_supported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device->physical, i, surface, &present_supported);
		if (queue_families[i].queueCount > 0 && present_supported) {
			*present_family = i;
			found_present = true;
		}

		if (found_graphics && found_present) break;
	}

	free(queue_families);
	return found_graphics && found_present;
}

static bool check_device_extension_support(vkdev_t* device) {
	const char* required_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	uint32_t ext_count;
	vkEnumerateDeviceExtensionProperties(device->physical, NULL, &ext_count, NULL);

	VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * ext_count);
	vkEnumerateDeviceExtensionProperties(device->physical, NULL, &ext_count, extensions);

	bool found = false;
	for (uint32_t i = 0; i < ext_count; i++) {
		if (strcmp(extensions[i].extensionName, required_extensions[0]) == 0) {
			found = true;
			break;
		}
	}
	free(extensions);
	return found;
}

static bool is_device_suitable(vkdev_t* device, VkSurfaceKHR surface) {
	uint32_t graphics_family = 0, present_family = 0;
	if (!get_queue_families(device, surface, &graphics_family, &present_family)) return false;
	if (!check_device_extension_support(device)) return false;

	device->graphics_family = graphics_family;
	device->present_family = present_family;

	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(device->physical, &supported_features);

	return supported_features.samplerAnisotropy;
}

vkdev_t* gfx_device_get(win_t* win) {
	if (device.initialized) return &device;
	if (!win) {
		printf("s");
	}
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(gfx_vkinstance(), &device_count, NULL);
	if (device_count == 0) { debug_err("No Vulkan-supported GPUs found"); return NULL; }

	VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * device_count);
	vkEnumeratePhysicalDevices(gfx_vkinstance(), &device_count, devices);

	for (uint32_t i = 0; i < device_count; i++) {
		device.physical = devices[i];
		if (is_device_suitable(&device, win->surface)) break;
	}
	free(devices);

	float queue_priority = 1.0f;
	uint32_t unique_queues[] = { device.graphics_family, device.present_family };
	uint32_t unique_count = (device.graphics_family == device.present_family) ? 1 : 2;

	VkDeviceQueueCreateInfo queue_infos[2] = { 0 };
	for (uint32_t i = 0; i < unique_count; ++i) {
		queue_infos[i] = (VkDeviceQueueCreateInfo){
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = unique_queues[i],
			.queueCount = 1,
			.pQueuePriorities = &queue_priority
		};
	}

	VkPhysicalDeviceFeatures device_features = { 0 };
	device_features.samplerAnisotropy = VK_TRUE;

	const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = unique_count,
		.pQueueCreateInfos = queue_infos,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = extensions,
		.pEnabledFeatures = &device_features,
	};

	VK_CHECK(vkCreateDevice(device.physical, &create_info, NULL, &device.device));
	vkGetDeviceQueue(device.device, device.graphics_family, 0, &device.graphics_queue);
	vkGetDeviceQueue(device.device, device.present_family, 0, &device.present_queue);

	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = device.graphics_family,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	};

	VK_CHECK(vkCreateCommandPool(device.device, &pool_info, NULL, &device.cmd_pool));

	VkCommandBufferAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = device.cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT,
	};
	VK_CHECK(vkAllocateCommandBuffers(device.device, &alloc_info, device.cmd_buffers));

	VkSurfaceCapabilitiesKHR capabilities = { 0 };
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical, win->surface, &capabilities);

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, win->surface, &format_count, NULL);

	if (format_count != 0) {
		device.available_formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
		device.available_formats_count = format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, win->surface, &format_count, device.available_formats);
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical, win->surface, &present_mode_count, NULL);

	if (present_mode_count != 0) {
		device.available_present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
		device.available_present_modes_count = present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device.physical,
			win->surface,
			&present_mode_count,
			device.available_present_modes);
	}


	device.initialized = true;
	return &device;
}

void gfx_device_del() {
	if (!device.initialized) return;

	if (device.available_formats) {
		free(device.available_formats);
		device.available_formats_count = 0;
		device.available_formats = NULL;

	}

	if (device.available_present_modes) {
		free(device.available_present_modes);
		device.available_present_modes_count = 0;
		device.available_present_modes = NULL;
	}

	vkFreeCommandBuffers(device.device, device.cmd_pool, MAX_FRAMES_IN_FLIGHT, device.cmd_buffers);
	vkDestroyCommandPool(device.device, device.cmd_pool, NULL);
	vkDestroyDevice(device.device, NULL);

	device = (vkdev_t){ 0 }; 
}