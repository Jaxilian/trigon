#include "window.h"
#include <SDL3/SDL.h>
#include "trigon/core/graphics/vkdevice.h"

bool window_t::_initialized = false;

window_t::window_t() {
	if (!_initialized) {
		SDL_Init(SDL_INIT_VIDEO);
		_initialized = true;
	}

	handle = SDL_CreateWindow("window", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	vkdevice_t::get();
}

window_t::~window_t() {
	SDL_DestroyWindow((SDL_Window*)handle);

	if (_initialized) {
		SDL_Quit();
	}
}

void window_t::update() {

}

