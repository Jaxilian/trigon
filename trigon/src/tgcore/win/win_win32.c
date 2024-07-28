#include "win.h"
#define SHOW_VULKAN
#include "tgcore/rdr/vkdef.h"


typedef struct {
	uint32_t		id;
	uint32_t		width;
	uint32_t		height;
	vptr_t			handle;
	VkSurfaceKHR	surface;
} win_data_t;

static win_data_t main_window = { 0 };

uint32_t win_new(cstr_t name) {
	if (main_window.handle != NULL) {

		return main_window.id;
	}

	return main_window.id;
}

void win_del(uint32_t id) {

}

void win_get_extent(uint32_t id, vec2 out) {

}

void win_get_position(uint32_t id, vec2 out) {

}

uint32_t win_get_aspect(uint32_t id) {
	return 0;
}

vptr_t win_get_handle() {
	return 0;
}

void service_win_start() {

}
void service_win_stop() {

}

