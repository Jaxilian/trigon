#include "vk.h"

void vk_new() {
	vk_device_new();
	vk_swapchain_new();
}

void vk_del() {
	vkDeviceWaitIdle(ctx->device.device);
	//vk_pipeline_layout_clear();
	vk_shader_clear();
	vk_pipeline_clear();
	vk_swapchain_del();
	vk_device_del();
}