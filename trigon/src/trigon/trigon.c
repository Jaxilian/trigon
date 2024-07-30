#include "trigon.h"
#include "cstd/cstd.h"

#include "services/internal/srv.h"

static bool running = false;

void trigon_quit(app_info_t app_info) {
	running = false;
}

static void trigon_onload(app_info_t* app_info) {
	debug_log_clear();
	services_set(app_info->services, SERVICE_STATE_START);

	if (services_active(SERVICE_RENDERER)) {
		running = true;
	}

	if (app_info->on_load)app_info->on_load();
}

static void trigon_onstart(app_info_t* app_info) {
	if (app_info->on_start) app_info->on_start();
}

static void trigon_onupdate(app_info_t* app_info) {
	if (services_active(SERVICE_RENDERER)) {
		srv_renderer_event_poll();
	}

	if (app_info->on_preupdate) app_info->on_preupdate();

	services_set(app_info->services, SERVICE_STATE_UPDATE);

	if (app_info->on_update) app_info->on_update();
}



static void trigon_ondraw(app_info_t* app_info) {
	if (!services_active(SERVICE_RENDERER)) return;

	srv_renderer_frame_begin();
	services_set(app_info->services, SERVICE_STATE_RENDER);
	srv_renderer_frame_end();

}

void trigon_start(app_info_t* app_info) {

	trigon_onload(app_info);
	trigon_onstart(app_info);

	while (running){
		trigon_onupdate(app_info);
		trigon_ondraw(app_info);
	}

	if (app_info->on_quit) app_info->on_quit();
	services_set(app_info->services, SERVICE_STATE_STOP);
	debug_log("Shutdown successful!\n");
}


