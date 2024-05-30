#include "app.h"
#include "stdio.h"
#include <memory.h>

 bool app_t::config_added	= false;
 app_config_t app_t::config = {};

version_t app_t::engine_version = version_t(1,0,0,0);

app_t::app_t() {
	if (!config_added) {
		debug_t::err("app_t tried to init but a config was never loaded!\n");
	}
}

app_t::~app_t() {
	config_added = false;
	config = {};
}

void app_t::load(const app_config_t& conf) {
	memcpy_s(&config, sizeof(app_config_t), &conf, sizeof(app_config_t)); 
	config_added = true;
}

