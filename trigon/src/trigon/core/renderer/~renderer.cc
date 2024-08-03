#include "renderer.h"

renderer_t::~renderer_t() {
	vkDestroyInstance(vkinst, NULL);
};