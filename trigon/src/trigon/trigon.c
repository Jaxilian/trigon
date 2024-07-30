#include "trigon.h"
#include "cstd/cstd.h"

#include "services/internal/srv.h"

static bool running = false;

void trigon_quit() {
	running = false;
}

void trigon_start(SERVICE_FLAGS services) {
	debug_log_clear();

	services_set(services, SERVICE_STATE_START);

	if(services_active(SERVICE_RENDERER)) running = true;

	while (running){
		if (services_active(SERVICE_RENDERER)) srv_renderer_event_poll();


		services_set(services, SERVICE_STATE_UPDATE);


		if (services_active(SERVICE_RENDERER)) srv_renderer_frame_begin();


		services_set(services, SERVICE_STATE_RENDER);


		if (services_active(SERVICE_RENDERER)) srv_renderer_frame_end();
	}

	services_set(services, SERVICE_STATE_STOP);
	debug_log("Shutdown successful!\n");
}


