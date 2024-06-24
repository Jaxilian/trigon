#include "trigon/app.h"

int main() {
	app_config_t config = {};
	config.app_name = "Engine";
	config.version = version_t(1, 0, 0, 0);
	app_t::load(config);

	app_t::get().start();



	
	return 0;
}