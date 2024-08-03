#ifndef TG_VK_DEF
#define TG_VK_DEF
#ifdef _USING_VULKAN_SDK
#include "trigon/core/std/std.h"
#include <vulkan/vulkan.h>

struct swap_support_t {
	VkSurfaceCapabilitiesKHR	capabilities	= {};
	VkSurfaceFormatKHR*			formats			= NULL;
	VkPresentModeKHR*			present_modes	= NULL;
	u32 format_count	= 0;
	u32 present_count	= 0;

	swap_support_t(u32 _format_count, u32 _present_count);
	~swap_support_t();
};

struct queue_family_indices_t {
	u32 graphics_family	= 0;
	u32 present_family	= 0;
	bool graphics_valued = false;
	bool present_valued = false;
	bool completed() { return graphics_valued && present_valued; }
};

#else
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkInstance;
typedef vptr_t VkDebugUtilsMessengerEXT;
typedef vptr_t VkPhysicalDevice;
typedef vptr_t VkCommandPool;
typedef vptr_t VkDevice;
typedef vptr_t VkQueue;
typedef vptr_t VkCommandBuffer;
typedef vptr_t VkSwapchainKHR;
typedef vptr_t VkImage;
typedef vptr_t VkImageView;
typedef vptr_t VkFormat;
typedef vptr_t VkExtent2D;
typedef vptr_t VkRenderPass;
typedef vptr_t VkDeviceMemory;
typedef vptr_t VkFramebuffer;
typedef vptr_t VkSemaphore;
typedef vptr_t VkFence;
typedef vptr_t VkPresentModeKHR;
typedef vptr_t VkShaderModule;
typedef vptr_t VkPipelineLayout;
typedef vptr_t VkPipeline;
typedef vptr_t VkSurfaceFormatKHR;
typedef vptr_t VkBuffer;
typedef uint32_t VkResult;
typedef uint32_t VkMemoryPropertyFlags;
typedef uint32_t VkDeviceSize;
typedef uint32_t VkBufferUsageFlags;
typedef uint32_t VkImageTiling;
typedef uint32_t VkImageUsageFlags;
typedef uint32_t VkImageLayout;
#endif

struct vkdev_t {
	VkInstance					instance	= VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT	dbg_msg		= VK_NULL_HANDLE;
	VkPhysicalDevice			physical	= VK_NULL_HANDLE;
	VkCommandPool				cmd_pool	= VK_NULL_HANDLE;
	VkQueue						gqueue		= VK_NULL_HANDLE;
	VkQueue						pqueue		= VK_NULL_HANDLE;
};

#endif // !TG_VK_DEF