#include "vk.h"

void vk_new() {
	vk_device_new();
	vk_swapchain_new();
}

void vk_del() {
	vkDeviceWaitIdle(ctx->device.device);
	vk_swapchain_del();
	vk_device_del();
}