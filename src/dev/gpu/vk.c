#include "vk.h"

void vk_new() {
	vk_device_new();
}

void vk_del() {
	vkDeviceWaitIdle(ctx->device.device);
	vk_device_del();
}