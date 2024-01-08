#ifndef VKL_DEF_H
#define VKL_DEF_H
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_DESC_PER_SET 8

typedef void(*vkl_create_surface_callback)(void* vkl_device_ptr);

typedef enum {
	ERROR_FATAL,
	ERROR_WARNING
} vkl_error_enum;

/* ------------------- VKL_SWAP_SUPPORT ------------------- */

typedef struct {
	VkSurfaceCapabilitiesKHR	capabilities;
	VkSurfaceFormatKHR*			formats;
	uint32_t					format_count;
	VkPresentModeKHR*			present_modes;
	uint32_t					present_count;
} vkl_swap_support_t;

/* ------------------- VKL_INSTANCE ------------------- */

typedef struct {
	const char*		app_name;
	const char*		eng_name;
	uint32_t		app_version;
	uint32_t		eng_version;
	uint32_t		vk_version;

	const char**    instance_ext;
	uint32_t		instance_ext_count;

	const char**	opt_validation_layers;
	uint32_t		opt_validation_layer_count;
} vkl_instance_info_t;

/* ------------------- VKL_DEVICE ------------------- */

typedef struct {
	char**		device_extensions;
	uint32_t	device_extension_count;

} vkl_device_info_t;

typedef struct {
	VkInstance					instance;
	VkDevice					device;
	VkPhysicalDevice			physical_device;
	VkPhysicalDeviceProperties	physical_props;
	VkQueue						graphics_queue;
	VkQueue						present_queue;
	uint32_t					graphics_family;
	uint32_t					present_family;
	VkCommandPool				command_pool;
	VkCommandBuffer				command_buffers[MAX_FRAMES_IN_FLIGHT];
	vkl_swap_support_t			swap_support;
	VkSurfaceKHR				surface;
	uint32_t					surface_width;
	uint32_t					surface_height;
	VkDebugUtilsMessengerEXT	debug_messenger;
} vkl_device_t;


/* ------------------- VKL_SWAPCHAIN ------------------- */

typedef struct {
	vkl_device_t*		device_ptr;
	VkPresentModeKHR	prefered_present_mode;
	VkExtent2D			surface_extent;
} vkl_swapchain_info_t;


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
} vkl_swapchain_t;

/* ------------------- VKL_STATE ------------------- */

typedef struct {
	vkl_swapchain_t*	bound_swapchain;
	vkl_device_t*		bound_device;
} vkl_state_info_t;

typedef struct {
	vkl_device_t*		device;
	vkl_swapchain_t*	swapchain;
	uint32_t			frame_index;
	uint32_t			image_index;
	bool				frame_in_progress;
} vkl_state_t;

typedef enum {
	SHADER_STAGE_DEFAULT,
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_VERTEX
} vkl_shader_stage_e;

typedef enum {
	SHADER_STATIC_BUFFER,
	SHADER_DYNAMIC_BUFFER,
	SHADER_SAMPLER_BUFFER
} vkl_shader_property_e;

typedef struct {
	bool							initialized;
	uint32_t						location;
	uint32_t						count;
	VkDescriptorSetLayoutBinding	binding;
	vkl_shader_property_e			type;
} vkl_descriptor_t;


typedef struct {
	bool							initialized;
	uint32_t						descriptor_count;
	vkl_descriptor_t				descriptors[MAX_DESC_PER_SET];
	VkDescriptorPool				pool;
	VkDescriptorSet					set;
	VkDescriptorSetLayout			set_layout;
} vkl_descriptor_set_t;

typedef struct {
	vkl_device_t*		device;
	size_t				stride;
	uint32_t			count;
	VkBufferUsageFlags	usage_flags;
} vkl_buffer_info_t;

typedef struct {
	bool				initialized;
	VkBufferUsageFlags	usage_flags;
	vkl_device_t*		dev_ptr;
	VkBuffer			buffer;
	VkDeviceMemory		memory;
	VkDeviceSize		memory_size;
	size_t				size;	
	size_t				stride; 
	uint32_t			count;
} vkl_buffer_t;


#ifdef __cplusplus
}
#endif

#endif // !VKL_MAIN_H
