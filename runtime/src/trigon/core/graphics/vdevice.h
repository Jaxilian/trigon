#pragma once
#include <inttypes.h>
#include <vulkan/vulkan.h>
#include <vector>

struct queue_family_t {
	uint32_t present_family = 0;
	uint32_t graphics_family = 0;
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
};



class vdevice_t {
public:

	void bind_device(VkSurfaceKHR surface);

	VkInstance					instance		= nullptr;
	VkDebugUtilsMessengerEXT	debugger		= nullptr;
	VkPhysicalDevice			physical		= nullptr;
	VkCommandPool				cmd_pool		= nullptr;
	VkDevice					device			= nullptr;
	VkQueue						graphics_queue	= nullptr;
	VkQueue						present_queue	= nullptr;
	queue_family_t				queue_family	= {};

private:

	vdevice_t();
	~vdevice_t();


	void create_instance();
	void get_physical_device(VkSurfaceKHR surface);

	bool device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
	void get_queue_families(
		VkPhysicalDevice physical_device,
		bool& success,
		uint32_t& g_family,
		uint32_t& p_family,
		VkSurfaceKHR surface
	);

	VkFormat find_format(
		const std::vector<VkFormat>&candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features
	);



public: //singleton
	vdevice_t(const vdevice_t&) = delete;
	vdevice_t& operator=(const vdevice_t&) = delete;
	static inline vdevice_t& get() {
		static vdevice_t i;
		return i;
	}

};