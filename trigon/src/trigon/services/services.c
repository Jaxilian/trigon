#include "services.h"
#include "tgcore/win/win.h"
#include "tgcore/rdr/backend/vk.h"
#include "internal/srv.h"



static uint32_t active_services = 0;

static cstr_t STATE_NAME[] = {
	[SERVICE_STATE_NONE]	= "NONE   ",
	[SERVICE_STATE_START]	= "START",
	[SERVICE_STATE_STOP]	= "STOPS",
	[SERVICE_STATE_UPDATE]	= "UPDATE ",
	[SERVICE_STATE_RENDER]	= "RENDER ",
	[SERVICE_STATE_COUNT]	= "INVALID"
};

static service_t services[SERVICE_COUNT] = {
	[SERVICE_RENDERER] = {
		.name = "Renderer",
		.event_cbs = {
			[SERVICE_STATE_START] = srv_renderer_start,
			[SERVICE_STATE_STOP] = srv_renderer_stop,
		}
	},
};

void services_set(SERVICE_FLAGS sids, SERVICE_STATE state) {
	for (uint32_t i = 1; i < SERVICE_COUNT; i++) {
		uint32_t enum_id = (i - 1);

		if (!FLAG_ACTIVE(sids, (1 << enum_id))) continue;

		if (state == SERVICE_STATE_START) {
			if (FLAG_ACTIVE(active_services, (1 << enum_id))) continue;
			FLAG_ON(active_services, (1 << enum_id));
		}
		else if (state == SERVICE_STATE_STOP) {
			if (!FLAG_ACTIVE(active_services, (1 << enum_id))) continue;
			FLAG_OFF(active_services, (1 << enum_id));
		}
		else {
			if (!FLAG_ACTIVE(active_services, (1 << enum_id))) continue;
		}

		if (services[i].event_cbs[state]) {
			services[i].event_cbs[state]();
		}

		if (state == SERVICE_STATE_RENDER || state == SERVICE_STATE_UPDATE) {
			continue;
		}

		debug_log("Service [STATE:%s] : %s \n", 
			STATE_NAME[state],
			services[i].name
		);
	}
}


bool services_active(SERVICE service) {
	return FLAG_ACTIVE(active_services, service);
}
