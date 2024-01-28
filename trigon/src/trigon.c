#include "trigon.h"
#include "core/core.h"
#include "trigui/gui.h"
#include "managers/managers.h"
#include <vkl/vkl.h>

static void on_draw() {

}

static void on_ui_draw() {
	bool active = true;
	if (gui_window_new("test!", &active)) {
		gui_text("hello world!\n");

		
	}
	gui_window_end();
}

static void trigon_start() {
	signal_init();
	trigon_setup_events();
	trigon_core_init(0);


	managers_new();

	trigon_core_start(on_draw, on_ui_draw);

	vkl_device_t* dev = (vkl_device_t*)trigon_core_vkl_device();
	vkDeviceWaitIdle(dev->device);
	managers_del();

	trigon_core_del();
}

#ifdef _DEBUG
int main() {
	trigon_start();
	return 0;
}
#else

#ifdef _WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	trigon_start();
	return 0;
}

#else
int main() {
	trigon_start();
	return 0;
}
#endif
#endif
