#include "rendr.h"

static rendr_t* handle = NULL;
static VkResult success = VK_FALSE;

static void on_win_resize() {
	swapchain_new(&handle->device, &handle->swapchain, handle->window.width, handle->window.height);
}

rendr_t* rendr_get() {
	return handle;
}

void rendr_new(rendr_t* rendr) {
	if (handle != NULL) debug_err("rendr already created\n");

	handle = rendr;
	vkinstance_new(&rendr->device);
	win_new(&rendr->window);
	vkdevice_new(&rendr->device, rendr->window.surface);

	swapchain_new(&rendr->device, &rendr->swapchain, handle->window.width, handle->window.height);
	win_event_resized(on_win_resize);
	vkstate_new(&rendr->state, &rendr->device, &rendr->swapchain);

}

void rendr_upd(rendr_t* rendr) {
	win_upd(&rendr->window);
}

void rendr_bgn(rendr_t* rendr) {
	success = vkstate_frame_begin(&rendr->state);
}

void rendr_end(rendr_t* rendr) {
	if (success == VK_SUCCESS) {
		vkstate_frame_end(&rendr->state);
	}
}

void rendr_del(rendr_t* rendr) {
	vkDeviceWaitIdle(rendr->device.device);
	vkstate_del(&rendr->state);
	swapchain_del(&rendr->device, &rendr->swapchain);
	vkdevice_del(&rendr->device);
	win_del(&rendr->window);
	vkinstance_del(&rendr->device);
}