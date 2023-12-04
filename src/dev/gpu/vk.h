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

uint32_t	vk_pipeline_new(vk_descriptor_set_t* sets[MAX_DESC_SETS_IN_USE], uint32_t sets_count, vk_shader_t* shader, vk_pipeline_config_t* config);
void		vk_pipeline_del(uint32_t id);
void		vk_pipeline_clear();
void		vk_pipeline_bind(vk_pipeline_t* pipeline);
void		vk_pipeline_rebuild();
vk_pipeline_t* vk_pipeline_get(uint32_t id);

void vk_shader_new(vk_shader_t* shader, cstr vertex_path, cstr fragment_path);
void vk_shader_del(vk_shader_t* shader);
void vk_shader_clear();

void vk_descriptor_new(uint32_t location, uint32_t count, shader_property_e type, shader_stage_e stage, vk_descriptor_t* out);
void vk_descriptor_set_new(vk_descriptor_t descriptors[MAX_DESC_PER_SET], uint32_t count, vk_descriptor_set_t* out);
void vk_descriptor_set_del(vk_descriptor_set_t* in);

void  vk_buffer_new(size_t stride, uint32_t count, vk_buffer_t* out, VkBufferUsageFlags flags);
void  vk_buffer_resize(vk_buffer_t* buffer, uint32_t new_count);
void  vk_buffer_set(vk_buffer_t* buffer, void* data);
void* vk_buffer_set_advanced(vk_buffer_t* buffer);
void  vk_buffer_set_advanced_submit(vk_buffer_t* buffer, void* mapped_mem);
void  vk_buffer_del(vk_buffer_t* buffer);
void  vk_buffer_clear(vk_buffer_t* arr, uint32_t count);

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

VkDescriptorType vk_descriptor_convert_type(shader_property_e type);


#endif