#include "srv.h"
#include "tgcore/win/win.h"
#include "tgcore/rdr/backend/vk.h"
#include "trigon/trigon.h"
typedef struct {
	uint32_t		win_id;
	VkSurfaceKHR	surface;
	vkswapchain_t	swapchain;
} window_t;

typedef struct {
	vkdevice_t		device;
	vkstate_t		state;
} renderer_t;

static window_t		main_window = { 0 };
static renderer_t	renderer	= { 0 };
static VkResult		frame_started = false;

void win_event_cb(uint32_t win_id, WIN_EVENT event) {
	switch (event) {
	case WIN_EVENT_RESIZE:
		vec2 win_extent = { 0 };
		win_get_extent(win_id, win_extent);

		swapchain_new(
			&renderer.device,
			&main_window.swapchain,
			(uint32_t)win_extent[0],
			(uint32_t)win_extent[1]
		);
		break;
	case WIN_EVENT_DELETE:
		if (win_id == main_window.win_id) {
			trigon_quit();
		}
		break;
	case WIN_EVENT_CREATE:

		break;
	}
}

void srv_renderer_start() {
	main_window.win_id = win_new("Trigon");
	win_event_callback(win_event_cb);
	
	vkinstance_new(&renderer.device);
	win_surface(
		main_window.win_id,
		renderer.device.instance,
		&main_window.surface
	);

	vkdevice_new(&renderer.device, main_window.surface);

	vec2 extent = { 0 };
	win_get_extent(main_window.win_id, extent);

	swapchain_new(
		&renderer.device,
		&main_window.swapchain,
		(uint32_t)extent[0],
		(uint32_t)extent[1]
	);

	vkstate_new(&renderer.state, &renderer.device, &main_window.swapchain);

}

void srv_renderer_stop() {
	vkDeviceWaitIdle(renderer.device.device);
	vkstate_del(&renderer.state);
	swapchain_del(&renderer.device, &main_window.swapchain);
	vkDestroySurfaceKHR(renderer.device.instance, main_window.surface, NULL);
	vkdevice_del(&renderer.device);
	vkinstance_del(&renderer.device);
	win_del(main_window.win_id);
}

void srv_renderer_event_poll() {
	win_poll_events();
}

void srv_renderer_frame_begin() {
	frame_started = vkstate_frame_begin(&renderer.state);
}

void srv_renderer_frame_end() {
	if (frame_started == VK_SUCCESS) {
		vkstate_frame_end(&renderer.state);
	}
}