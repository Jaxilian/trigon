#include "loop_events.h"
#include <SDL3/SDL.h>
#include "trigon/app.h"

void sync_loop_events() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) { 

		switch (ev.type){
			case(SDL_EVENT_WINDOW_CLOSE_REQUESTED):
				app_t::get().quit();
				break;

			default:
				break;
		}

    }
}