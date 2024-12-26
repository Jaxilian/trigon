#ifndef TGN_GFXCTX_H
#define TGN_GFXCTX_H

#include <stdbool.h>

#define MAX_FRAMES_IN_FLIGHT 2

typedef void (*win_resize_cb) (void* win);
typedef void (*win_del_cb)(void* win);
/* -------------------- VULKAN EXPOSED DEFINITIONS ---------------------- */

#if _USING_VULKAN_SDK

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <shaderc/shaderc.h>

/* --------------------- VULKAN HIDDEN DEFINITIONS ---------------------- */
#else
typedef void* shaderc_shader_kind;
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
typedef void* VkShaderModule;
typedef void* VkPipelineLayout;
typedef void* VkPipeline;

typedef void* VkSurfaceFormatKHR;
typedef void* VkPresentModeKHR;
typedef bool  VkResult;

typedef void* VkPipelineViewportStateCreateInfo;
typedef void* VkPipelineInputAssemblyStateCreateInfo;
typedef void* VkPipelineRasterizationStateCreateInfo;
typedef void* VkPipelineMultisampleStateCreateInfo;
typedef void* VkPipelineColorBlendAttachmentState;
typedef void* VkPipelineColorBlendStateCreateInfo;
typedef void* VkPipelineDepthStencilStateCreateInfo;
typedef void* VkPipelineDynamicStateCreateInfo;
typedef void* VkDescriptorSetLayoutBinding;
typedef void* VkDescriptorSetLayout;
typedef void* VkDynamicState;
typedef void* VkVertexInputBindingDescription;
typedef void* VkVertexInputAttributeDescription;

#endif

/* ------------------------ COMMON INFORMATION -------------------------- */

typedef struct {
	bool						initialized;
	VkPhysicalDevice			physical;
	VkCommandPool				cmd_pool;
	VkCommandBuffer				cmd_buffers[MAX_FRAMES_IN_FLIGHT];
	VkDevice					device;
	VkQueue						graphics_queue;
	VkQueue						present_queue;
	uint32_t					queue_count;
	uint32_t					present_family;
	uint32_t					graphics_family;

	uint32_t					available_formats_count;
	VkSurfaceFormatKHR*			available_formats;

	uint32_t					available_present_modes_count;
	VkPresentModeKHR*			available_present_modes;
} vkdev_t;

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
} vkswap_t;

typedef struct {

	void* handle;
	void* instance;

	uint32_t width;
	uint32_t height;
	uint32_t current_frame;
	uint32_t frame_index;
	uint32_t image_index;

	float	aspect;
	bool	active;
	bool	frame_in_process;
	bool	frame_ready;
	float	color[4];

	VkSurfaceKHR	surface;
	win_resize_cb	resize_cb;
	win_del_cb		delete_cb;
	vkdev_t*		device;
	vkswap_t		swapchain;
} win_t;



/* -------------------- VULKAN EXPOSED FUNCTIONS ---------------------- */


#if _USING_VULKAN_SDK
#define gfx_dev() gfx_device_get(NULL)
vkdev_t*	gfx_device_get(win_t* win);
void		gfx_device_del();

VkInstance	gfx_vkinstance();
void		gfx_swap_sync(win_t* win);
void		gfx_swap_del(win_t* win);
bool		gfx_frame_begin(win_t* win);
VkResult	gfx_frame_end(win_t* win);
void		gfx_sync_objects_create(win_t* win);
void		gfx_sync_objects_destroy(win_t* win);


VkCommandBuffer gfx_cmdbuff(win_t* win);

VkCommandBuffer gfx_cmd_begin();
void gfx_cmd_end(VkCommandBuffer cmd_buff);
uint32_t gfx_find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
#endif


#endif // !TGN_GFXCTX_H
