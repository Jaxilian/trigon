#ifndef VKL_MAIN_H
#include "vkl_def.h"


#ifdef __cplusplus
extern "C" {
#include <stdnoreturn.h>
#include <cinttypes>
#define _noreturn [[noreturn]] 
#else 
#define _noreturn _Noreturn
#endif

_noreturn void vkl_error(const char* msg, vkl_error_enum type);

void vkl_instance_new(const vkl_instance_info_t* info, vkl_device_t* device);

void vkl_device_new(const vkl_device_info_t* info, vkl_device_t* device);
void vkl_device_del(vkl_device_t* device);

void vkl_swapchain_new(const vkl_swapchain_info_t* info, vkl_swapchain_t* swapchain);
void vkl_swapchain_del(vkl_device_t* device, vkl_swapchain_t* swapchain);

void vkl_reset_command_pool(vkl_device_t* device);

VkFormat vkl_format_supported(
	const vkl_device_t* device,
	const VkFormat* candidates,
	uint32_t candidate_count,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
);

uint32_t vkl_fetch_memory_type(
	const vkl_device_t* device,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties
);
void vk_create_image_with_info(
	const vkl_device_t* device,
	const VkImageCreateInfo* info,
	VkMemoryPropertyFlags properties,
	VkImage* image,
	VkDeviceMemory* image_memory
);

VkResult		vkl_state_next_frame(vkl_state_t* state);
VkCommandBuffer vkl_state_command_buffer(vkl_state_t* state);
VkResult		vkl_state_submit_commands(vkl_state_t* state);
VkCommandBuffer vkl_state_single_cmd(vkl_state_t* state);
void			vkl_state_single_cmd_submit(vkl_state_t* state, VkCommandBuffer cmd_buff);
VkResult		vkl_state_frame_begin(vkl_state_t* state);
VkResult		vkl_state_frame_end(vkl_state_t* state);
void			vkl_state_new(vkl_state_info_t* info, vkl_state_t* state);
void			vkl_state_del(vkl_state_t* state);

uint32_t vkl_get_memory_type(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

/*
void vk_descriptor_new(uint32_t location, uint32_t count, vkl_shader_property_e type, vkl_shader_stage_e stage, vkl_descriptor_t* out);
void vk_descriptor_set_new(vkl_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count, vkl_descriptor_set_t* out);
void vk_descriptor_set_del(vkl_descriptor_set_t* in);
*/

#ifdef __cplusplus
}
#endif





#endif // !VKL_MAIN_H
