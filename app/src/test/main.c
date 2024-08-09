#include "trigon/shaders/shader.h"
#include "trigon/gui/gui.h"
#include "trigon/trigon.h"

u32 shader;

void app_start() {
	shader_info_t shainf = { 
		.name = "default2d",

	};

	shader_new(&shainf);

	
}

void app_update() {

}

void app_quit() {

}

app_info_t app_main() {
	return (app_info_t) {
		.app_name		= "Editor",
		.app_version	= (version_t){1,0,0,0},
		.app_start_cb	= app_start,
		.app_update_cb	= app_update,
		.app_quit_cb	= app_quit
	};
}
