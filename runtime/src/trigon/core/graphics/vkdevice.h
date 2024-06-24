#pragma once

#ifdef VK_EXPOSED
#include <vulkan/vulkan.h>
#else
typedef void* VkInstance;
typedef void* VkDevice;
typedef void* VkPhysicalDevice;
typedef void* VkCommandPool;
typedef void* VkQueue;
typedef void* VkDebugUtilsMessengerEXT;
#endif

class vkdevice_t {
public:
	VkInstance			instance		= nullptr;
	VkDevice			device			= nullptr;
	VkPhysicalDevice	physical		= nullptr;
	VkCommandPool		cmd_pool		= nullptr;
	VkQueue				graphics_queue	= nullptr;
	VkQueue				present_queue	= nullptr;

	VkDebugUtilsMessengerEXT debugger						= nullptr;

private:

	vkdevice_t();
	~vkdevice_t();



	bool _running = false;

public: //singleton
	vkdevice_t(const vkdevice_t&) = delete;
	vkdevice_t& operator=(const vkdevice_t&) = delete;
	static inline vkdevice_t& get() {
		static vkdevice_t instance;
		return instance;
	}

};