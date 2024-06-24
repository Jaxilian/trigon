#ifndef TG_VKDEF_H
#define TG_VKDEF_H


#ifdef _VULKAN_EXPOSED
#include <vulkan/vulkan.h>
#else
typedef void* VkSurfaceKHR;
typedef void* VkInstance;
typedef void* VkDebugUtilsMessengerEXT;
typedef void* VkPhysicalDevice;
typedef void* VkCommandPool;
typedef void* VkDevice;
typedef void* VkQueue;
typedef void* VkCommandBuffer;
typedef void* VkSwapchainKHR;
typedef void* VkImage;
typedef void* VkImageView;
typedef void* VkFormat;
typedef void* VkExtent2D;
typedef void* VkRenderPass;
typedef void* VkDeviceMemory;
typedef void* VkFramebuffer;
typedef void* VkSemaphore;
typedef void* VkFence;
typedef void* VkPresentModeKHR;
#endif
#define MAX_FRAMES_IN_FLIGHT 2

typedef struct {
	VkInstance					instance;
	VkDebugUtilsMessengerEXT	debugger;
	VkPhysicalDevice			physical;
	VkCommandPool				cmd_pool;
	VkCommandBuffer				cmd_buffers[MAX_FRAMES_IN_FLIGHT];
	VkDevice					device;
	VkQueue						graphics_queue;
	VkQueue						present_queue;
	uint32_t					queue_count;
	uint32_t					present_family;
	uint32_t					graphics_family;
	VkSurfaceKHR				main_surface;

	uint32_t					available_formats_c;
	VkSurfaceFormatKHR*			available_formats;

	uint32_t					available_present_modes_c;
	VkPresentModeKHR*			available_present_modes;

} vkdevice_t;

typedef struct {
	VkSwapchainKHR	old_swap;
	VkSwapchainKHR	new_swap;
	VkImage*		images;
	VkImageView*	image_views;
	uint32_t		image_count;
	VkFormat		image_format;
	VkExtent2D		extent;
	VkRenderPass	renderpass;
	VkFormat        depth_format;
	VkImage*		depth_images;
	VkDeviceMemory* depth_memories;
	VkImageView*	depth_views;
	VkFramebuffer*	framebuffers;
	VkSemaphore     available_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore     finished_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence         in_flight_fences[MAX_FRAMES_IN_FLIGHT];
	VkFence*		images_in_flight;
	uint32_t		current_frame;
} vkswapchain_t;

void vkinstance_new(vkdevice_t* device);
void vkinstance_del(vkdevice_t* device);
void vkdevice_new(vkdevice_t* device, VkSurfaceKHR surface);
void vkdevice_del(vkdevice_t* device);

void swapchain_new(vkdevice_t* device, vkswapchain_t* swapchain, uint32_t extent[2]);
void swapchain_del(vkdevice_t* device, vkswapchain_t* swapchain);

#endif // !TG_VKDEF_H
