#ifndef CORE_POOL_H
#define CORE_POOL_H
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "dev/std/std.h"
#include "dev/cpu/threads.h"
#include <cglm/cglm.h>
#include "dev/gpu/vk_def.h"

typedef struct {
	vk_win_t			win;
	vk_app_data_t		app_data;
	vk_device_t			device;
	vk_swapchain_t		swapchain;
	vk_state_t			vk_state;
	vk_shader_t			shaders[MAX_SHADERS];
	vk_pipeline_t		pipelines[MAX_SHADERS];
	VkPipelineLayout	pipeline_layouts[MAX_SHADERS];
	vk_descriptor_t		descriptors[MAX_UNIQUE_DESC];
	vk_descriptor_set_t descriptor_sets[MAX_SHADER_SETS];
	vk_buffer_t			descriptor_buffers[MAX_SHADER_BUFFERS];

	double				tick_pre;
	double				tick_now;
	double				tick_dt;

} core_ctx_t;


void		core_ctx_new();

extern inline
core_ctx_t* core_ctx_get();
#define ctx core_ctx_get()

void		core_ctx_del();


#endif // !CORE_POOL_H
