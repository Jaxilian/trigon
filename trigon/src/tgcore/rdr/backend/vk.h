#ifndef TG_VK_H
#define TG_VK_H
#include "vkdef.h"
#include "cstd/cstd.h"

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct {
	VkImage			image;
	VkDeviceMemory	memory;
} vkimage_t;

typedef struct {
	VkDevice				device;
	VkPhysicalDevice		physical;
	uint32_t				width;
	uint32_t				height;
	VkFormat				format;
	VkImageTiling			tiling;
	VkImageUsageFlags		usage;
	VkMemoryPropertyFlags	properties;
} vkimage_info_t;

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
	VkSurfaceFormatKHR* available_formats;

	uint32_t					available_present_modes_c;
	VkPresentModeKHR* available_present_modes;

} vkdevice_t;

typedef struct {
	VkSwapchainKHR	old_swap;
	VkSwapchainKHR	new_swap;
	VkImage* images;
	VkImageView* image_views;
	uint32_t		image_count;
	VkFormat		image_format;
	VkExtent2D		extent;
	VkRenderPass	renderpass;
	VkFormat        depth_format;
	VkImage* depth_images;
	VkDeviceMemory* depth_memories;
	VkImageView* depth_views;
	VkFramebuffer* framebuffers;
	VkSemaphore     available_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore     finished_semaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence         in_flight_fences[MAX_FRAMES_IN_FLIGHT];
	VkFence* images_in_flight;
	uint32_t		current_frame;
} vkswapchain_t;

typedef struct {
	vkdevice_t* device;
	vkswapchain_t* swapchain;
	uint32_t			frame_index;
	uint32_t			image_index;
	bool				frame_in_progress;
} vkstate_t;

#ifdef _USING_VULKAN_SDK
typedef struct {
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineInputAssemblyStateCreateInfo	input_assembly;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineColorBlendAttachmentState		color_blend_attachment;
	VkPipelineColorBlendStateCreateInfo		color_blend_state;
	VkPipelineDepthStencilStateCreateInfo	depth_stencil;
	VkPipelineDynamicStateCreateInfo		dynamic_state;
	VkVertexInputBindingDescription*		vertex_binding;
	VkVertexInputAttributeDescription*		vertex_attribute;
	uint32_t								subpass;
	uint32_t								vertex_binding_count;
	uint32_t								vertex_attribute_count;
	bool									use3D;
} vkpipeline_conf_t;

void vkpipeline_conf_default(vkpipeline_conf_t* conf);
#endif


void vkinstance_new(vkdevice_t* device);
void vkinstance_del(vkdevice_t* device);

void vkdevice_new(vkdevice_t* device, VkSurfaceKHR surface);
void vkdevice_del(vkdevice_t* device);

void swapchain_new(vkdevice_t* device, vkswapchain_t* swapchain, uint32_t width, uint32_t height);
void swapchain_del(vkdevice_t* device, vkswapchain_t* swapchain);

void vkimage_new(vkimage_info_t* info, vkimage_t* out);
void vkimage_del(VkDevice device, vkimage_t* img);

void vkimage_transition_layout(
	vkstate_t* state,
	VkImage			image,
	VkFormat		format,
	VkImageLayout	oldlayout,
	VkImageLayout	newlayout
);
void vkimage_copy_buffer(
	vkstate_t* state,
	vkimage_t* to,
	VkBuffer src,
	uint32_t width,
	uint32_t height
);

VkResult		vkstate_next_frame(vkstate_t* state);
VkCommandBuffer vkstate_command_buffer(vkstate_t* state);
VkResult		vkstate_submit_commands(vkstate_t* state);
VkCommandBuffer vkstate_single_cmd(vkstate_t* state);
void			vkstate_single_cmd_submit(vkstate_t* state, VkCommandBuffer cmd_buff);
VkResult		vkstate_frame_begin(vkstate_t* state);
VkResult		vkstate_frame_end(vkstate_t* state);
void			vkstate_new(vkstate_t* state, vkdevice_t* device, vkswapchain_t* swap);
void			vkstate_del(vkstate_t* state);

uint32_t vk_memory_type(VkPhysicalDevice device, uint32_t type, VkMemoryPropertyFlags properties);

void vkbuffer_new(
	vkdevice_t* device,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer* buffer,
	VkDeviceMemory* memory
);

#endif // !TG_VK_H
