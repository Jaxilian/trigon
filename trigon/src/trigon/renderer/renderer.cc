#include "renderer.h"

renderer_t* renderer_t::instance = nullptr;

renderer_t::renderer_t(cstr_t app_name, version_t version)
	: device(app_name, version), swap(device), cmdr(device, swap) {
	instance = this;
}

renderer_t::~renderer_t() {

}