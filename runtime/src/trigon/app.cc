#include "app.h"
#include "stdio.h"
#include <memory.h>
#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include "trigon/math/matrix.h"
#include <iostream>


 bool app_t::config_added	= false;
 app_config_t app_t::config = {};

version_t app_t::engine_version = version_t(1,0,0,0);

app_t::app_t() {
	if (!config_added) {
		debug_t::err("app_t tried to init but a config was never loaded!\n");
	}

#if defined(_WIN32)
	char p[MAX_PATH];
	GetModuleFileNameA(nullptr, p, MAX_PATH);
#else
	char path[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
	if (count == -1) {
		throw std::runtime_error("Unable to get executable path");
	}
	path[count] = '\0';
#endif

	strcpy_s(app_path, sizeof(app_path), p);
	strcpy_s(current_path, sizeof(current_path), std::filesystem::current_path().string().data());

	

}

app_t::~app_t() {
	config_added = false;
	config = {};
}

void app_t::load(const app_config_t& conf) {
	memcpy_s(&config, sizeof(app_config_t), &conf, sizeof(app_config_t)); 
	config_added = true;
}
