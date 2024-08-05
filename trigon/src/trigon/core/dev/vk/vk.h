#ifndef TR_VK_H
#define TR_VK_H

#include "trigon/core/std/std.h"

#ifdef _USING_VULKAN_SDK
#include <vulkan/vulkan.h>
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
typedef u32 VkResult;
typedef u32 VkMemoryPropertyFlags;
typedef u32 VkDeviceSize;
typedef u32 VkBufferUsageFlags;
typedef u32 VkImageTiling;
typedef u32 VkImageUsageFlags;
typedef u32 VkImageLayout;
#endif

#define MAX_FRAMES_IN_FLIGHT 2
#define WIN_MAX_NAME_LEN 16

typedef struct {
	u32		extent[2];
	u32		position[2];
	vptr_t	handles[2];
} vkwin_t;

typedef struct {
	u32 gfam;
	u32 pfam;
	b8  gfam_valued;
	b8  pfam_valued;
}vk_qfi_t;

typedef struct {
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR* formats;
	VkPresentModeKHR* presents ;
	u32 format_count;
	u32 present_count;
} vk_ssd_t; // swapchain support details


typedef struct {
	VkInstance					instance;
	VkDebugUtilsMessengerEXT	debugger;
	VkPhysicalDevice			physical;
	VkCommandPool				cmd_pool;
	VkCommandBuffer				cmd_buffers[MAX_FRAMES_IN_FLIGHT];
	VkDevice					device;
	VkQueue						graphics_queue;
	VkQueue						present_queue;
	u32							queue_count;
	vk_qfi_t					qfi;
	vk_ssd_t					ssd;
	VkSurfaceKHR				surface;
} vkdev_t;

typedef struct {
	VkSwapchainKHR	old_swap;
	VkSwapchainKHR	new_swap;
	VkImage*		images;
	VkImageView*	image_views;
	u32				image_count;
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
	u32				current_frame;
} vkswap_t;

typedef struct {
	u32	frame_index;
	u32	image_index;
	b8	frame_in_progress;
} vkstate_t;

typedef struct {
	vkwin_t		win;
	vkdev_t		dev;
	vkswap_t	swap;
	vkstate_t	state;
	b8			win_closing;
} vkctx_t;

void		vkctx_new(cstr_t app_name, version_t app_version);
vkctx_t*	vkctx_get();
void		vkctx_del();
void		vkctx_poll();

VkCommandBuffer vkctx_current_cmd();
VkResult vkctx_frame_begin();
VkResult vkctx_frame_end();
#endif // !TR_VK_H
