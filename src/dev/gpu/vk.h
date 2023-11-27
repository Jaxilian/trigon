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

uint32_t	vk_pipeline_new(VkPipelineLayout* layout, vk_shader_t* shader, vk_pipeline_config_t* config);
void		vk_pipeline_del(uint32_t id);
void		vk_pipeline_clear();
void		vk_pipeline_bind(vk_pipeline_t* pipeline);
void		vk_pipeline_rebuild();
vk_pipeline_t* vk_pipeline_get(uint32_t id);

uint32_t	vk_pipeline_layout_new(uint32_t descriptors[MAX_DESCRIPTOR_SETS_IN_USE], uint32_t count);
void		vk_pipeline_layout_del(uint32_t id);
void		vk_pipeline_layout_clear();

void		vk_descriptors_bind(uint32_t descriptor, uint32_t pipeline_layout);
void		vk_descriptor_del(uint32_t id);
uint32_t	vk_descriptor_new(VkDescriptorSetLayoutBinding info[MAX_DESCRIPTORS_PER_SET], uint32_t count);
void		vk_descriptor_clear();
void		vk_descriptors_update(
				uint32_t descriptor_id,
				uint32_t binding_id,
				vk_buffer_t* buffers[MAX_DESCRIPTOR_SETS_IN_USE]
			);

void vk_buffer_new(size_t size, uint32_t count, vk_buffer_t* out);
void vk_buffer_resize(vk_buffer_t* buffer, uint32_t new_count);
void vk_buffer_set(vk_buffer_t* buffer, void* data);
void vk_buffer_del(vk_buffer_t* buffer);

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