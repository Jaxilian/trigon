#include "trigon.h"
#include "core/core.h"
#include "gui/gui.h"

static void on_draw() {
	gui_draw();
}

#ifdef _DEBUG
int main() {
	signal_init();
	trigon_setup_events();
	trigon_core_init();
	gui_new();

	mat4 test = { 0 };
	glm_mat4_identity(test);
	glm_mat4_scale(test, 1.0f);

	gui_add(test);

	trigon_core_start(on_draw);

	vkl_device_t* dev = (vkl_device_t * )trigon_core_vkl_device();
	vkDeviceWaitIdle(dev->device);
	gui_pipeline_del();
	trigon_core_del();
	return 0;
}
#else
#ifdef _WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	trigon_core_start();
	return 0;
}
#endif
#endif
