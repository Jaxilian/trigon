#ifndef T_VK_H
#define T_VK_H
#include "dev/mem/core_ctx_def.h"

void vk_device_new();
void vk_device_del();

void vk_frame_begin();
void vk_frame_end();

void vk_swapchain_new();
void vk_swapchain_del();

void vk_new();
void vk_del();

uint32_t vk_pipeline_new(VkPipelineLayout* layout, vk_shader_t* shader, vk_pipeline_config_t* config);
void vk_pipeline_del(uint32_t id);
void vk_pipeline_clear();
vk_pipeline_t* vk_pipeline_get(uint32_t id);
void vk_pipeline_bind(vk_pipeline_t* pipeline);
void vk_pipeline_rebuild();

VkCommandBuffer vk_command_buffer();

swap_support_t* vk_open_swap_support(VkPhysicalDevice device);
void vk_close_swap_support(swap_support_t* support);
uint32_t vk_fetch_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);

VkFormat vk_format_supported(
	const VkFormat* candidates,
	uint32_t candidate_count,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
);

void vk_create_image_with_info(
	const VkImageCreateInfo* info,
	VkMemoryPropertyFlags properties,
	VkImage* image,
	VkDeviceMemory* imageMemory);




#endif