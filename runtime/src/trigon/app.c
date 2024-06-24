#include "app.h"
#include <stdio.h>
#include "trigon/core/graphics/win.h"

static version_t trigon_version = { 1,0,0,0 };
static cstr_t	 trigon_name	= "trigon";

typedef struct {
	bool	 running;
	voidcb_t user_start;
	voidcb_t user_update;
	voidcb_t user_quit;
	uint32_t app_version;
	uint32_t eng_version;
	cstr_t	 app_name;
	win_t	 window;
} tgapp_t;

static tgapp_t app = { 0 };

void app_load(app_info_t* info) {
	app.user_start	= info->on_start;
	app.user_update = info->on_update;
	app.user_quit	= info->on_quit;
	app.app_version = version_to_int(info->version);
	app.eng_version = version_to_int(trigon_version);
	app.app_name	= info->name;

	debug_log("loaded: \"%s\" (version %d.%d.%d.%d)\n",
		trigon_name,
		trigon_version.version,
		trigon_version.major,
		trigon_version.minor,
		trigon_version.patch
	);

	debug_log("loaded: \"%s\" (version %d.%d.%d.%d)\n",
		app.app_name,
		info->version.version,
		info->version.major,
		info->version.minor,
		info->version.patch
	);

	win_new(&app.window);

	app.running = true;

	if(app.user_start) app.user_start();

	while (app.running && app.user_update) {
		win_upd(&app.window);
		app.user_update();
	}

	if(app.user_quit) app.user_quit();

	win_del(&app.window);
}

void		app_quit() {app.running = false;}
uint32_t	engine_version() {return app.eng_version;}
cstr_t		engine_name() { return trigon_name; }
cstr_t		app_name() {return app.app_name;}
uint32_t	app_version() {return app.app_version;}
