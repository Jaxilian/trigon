#include "trigon.h"
#include "core/core.h"
#include "gui/gui.h"
#include "shaders/shader_global.h"
#include <cglm/cglm.h>

static void on_draw() {
	gui_draw();
}

#ifdef _DEBUG
int main() {
	signal_init();
	trigon_setup_events();
	trigon_core_init();

	ivec2 extent = { 0 };
	trigon_core_win_extent(extent);

	shader_global_data_t gdata = { 0 };
	shader_global_data_get(&gdata);
	glm_vec2_copy((vec2) { (float)extent[0], (float)extent[1] }, gdata.win_extent);
	
	shader_global_data_set(&gdata);
	gui_new();

	gui_quad_t gui_data = { 0 };
	glm_mat4_identity(gui_data.matrix);
	glm_scale(gui_data.matrix, (vec3) { 1.0f, 1.0f, 1.0f });
	glm_translate(gui_data.matrix, (vec3) { -1.0f,0.0f,0.0f });
	glm_vec3_copy((vec3) { 1.0f, 1.0f, 0.0f }, gui_data.colors);
	gui_add(&gui_data);
	
	glm_mat4_identity(gui_data.matrix);
	glm_scale(gui_data.matrix, (vec3) { 1.0f, 1.0f, 1.0f });
	glm_translate(gui_data.matrix, (vec3) { 1.0f, 0.0f, 0.0f });
	glm_vec3_copy((vec3) {1.0f,1.0f,0.0f}, gui_data.colors);
	gui_add(&gui_data);

	gui_submit();

	trigon_core_start(on_draw);

	vkl_device_t* dev = (vkl_device_t*)trigon_core_vkl_device();
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
