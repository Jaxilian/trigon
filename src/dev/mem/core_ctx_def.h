#ifndef CORE_POOL_H
#define CORE_POOL_H
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "dev/std/std.h"
#include "dev/cpu/threads.h"

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct {
	bool		initialized;
	mutex_t		mutex;
	cstr		name;
	uint32_t	version;
} vk_app_data_t;

typedef struct {
	bool			initialized;
	bool			running;
	mutex_t			mutex;
	GLFWwindow*		handle;
	uint32_t		width;
	uint32_t		height;
	VkSurfaceKHR	surface;
} vk_win_t;

typedef struct {
	VkSurfaceCapabilitiesKHR	capabilities;
	VkSurfaceFormatKHR*			formats;
	uint32_t					format_count;
	VkPresentModeKHR*			present_modes;
	uint32_t					present_count;
} swap_support_t;

typedef struct {
	bool						initialized;
	mutex_t						mutex;
	VkInstance					instance;
	VkPhysicalDevice			gpu;
	VkDevice					device;
	VkPhysicalDeviceProperties	gpu_properties;
	VkQueue						graphics_queue;
	VkQueue						present_queue;
	uint32_t					graphics_family;
	uint32_t					present_family;
	VkCommandPool				command_pool;
	VkCommandBuffer				command_buffers[MAX_FRAMES_IN_FLIGHT];
} vk_device_t;

typedef struct {
	vk_win_t		win;
	vk_app_data_t	app_data;
	vk_device_t		device;
} core_ctx_t;


void		core_ctx_new();

extern inline
core_ctx_t* core_ctx_get();
#define ctx core_ctx_get()

void		core_ctx_del();


#endif // !CORE_POOL_H
