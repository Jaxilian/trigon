#include "trigon/app.h"

int main() {
	app_config_t config = {};
	config.app_name = "Engine";
	config.version = version_t(1, 0, 0, 0);
	app_t::load(config);

	while (!app_t::get().main_window.closing()) {
		app_t::get().main_window.update();
	}


	
	return 0;
}