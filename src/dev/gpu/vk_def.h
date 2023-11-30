#ifndef T_VK_DEF_H
#define T_VK_DEF_H
#include "dev/std/std.h"
#include "dev/mem/mem.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "dev/std/std.h"
#include "vk_cmn.h"


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
	bool			resized;
	double			prev_time;
	double			time_now;
	mutex_t			mutex;
	GLFWwindow* handle;
	uint32_t		width;
	uint32_t		height;
	VkSurfaceKHR	surface;
} vk_win_t;

typedef struct {
	VkSurfaceCapabilitiesKHR	capabilities;
	VkSurfaceFormatKHR* formats;
	uint32_t					format_count;
	VkPresentModeKHR* present_modes;
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
} vk_swapchain_t;

typedef struct {
	bool			initialized;
	VkShaderModule	vertex_module;
	VkShaderModule	fragment_module;
} vk_shader_t;

/*------------------ PIPELINE CONFIG -----------------------*/
#define default_dynamic_state_enable_count 2
static const VkDynamicState default_dynamic_states_enables[] = {
	VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
};

typedef struct {
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineInputAssemblyStateCreateInfo	input_assembly;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineColorBlendAttachmentState		color_blend_attachment;
	VkPipelineColorBlendStateCreateInfo		color_blend_state;
	VkPipelineDepthStencilStateCreateInfo	depth_stencil;
	VkPipelineDynamicStateCreateInfo		dynamic_state;
	VkVertexInputBindingDescription* vertex_binding;
	VkVertexInputAttributeDescription* vertex_attribute;
	uint32_t subpass;
	uint32_t vertex_binding_count;
	uint32_t vertex_attribute_count;
} vk_pipeline_config_t;


/*------------------ VERTEX ATTRIBUTES----------------------*/


#define default_vertex_binding_count 1
#define default_vertex_attribute_count 4
static const VkVertexInputBindingDescription default_vertex_binding[default_vertex_binding_count] = {
	{
	.binding = 0,
	.stride = sizeof(vertex_t),
	.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	}
};
static const VkVertexInputAttributeDescription default_vertex_attributes[default_vertex_attribute_count] = {
	{
		.binding = 0,
		.location = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(vertex_t, position)
	},
	{
		.binding = 0,
		.location = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(vertex_t, normal)
	},
	{
		.binding = 0,
		.location = 2,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(vertex_t, color)
	},
	{
		.binding = 0,
		.location = 3,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(vertex_t, uv)
	}
};

typedef struct {
	bool					initialized;
	vk_shader_t*			shader;
	vk_pipeline_config_t*	config;
	VkPipelineLayout		layout;
	VkPipeline				instance;
} vk_pipeline_t;

typedef struct {
	uint32_t	frame_index;
	uint32_t	image_index;
	bool		frame_in_progress;
} vk_state_t;

typedef struct {
	bool				initialized;
	VkBuffer			buffer;
	VkDeviceMemory		memory;
	VkDeviceSize		memory_size;
	size_t				size;	// size of the whole buffer
	size_t				stride; // size of each element
	uint32_t			count;	// count ot the elements
} vk_buffer_t;

typedef struct {
	bool							initialized;
	uint32_t						location;
	uint32_t						count;
	VkDescriptorSetLayoutBinding	binding;
	shader_property_e				type;
} vk_descriptor_t;


typedef struct {
	bool							initialized;
	uint32_t						descriptor_count;
	vk_descriptor_t					descriptors[MAX_DESC_PER_SET];
	VkDescriptorPool				pool;
	VkDescriptorSet					set;
	VkDescriptorSetLayout			set_layout;
} vk_descriptor_set_t;



#endif // !T_VK_DEF_H

