#ifndef TRIGON_H
#define TRIGON_H
#include "services/services.h"

typedef struct {
	cstr_t		name;
	version_t	version;
	vptrcb_t	on_load;
	vptrcb_t	on_start;
	vptrcb_t	on_preupdate;
	vptrcb_t	on_update;
	vptrcb_t	on_quit;

	SERVICE_FLAGS services;
} app_info_t;

void trigon_start(app_info_t* app_info);
void trigon_quit();

#endif // !TRIGON_H
