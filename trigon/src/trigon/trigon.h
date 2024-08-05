#ifndef TRIGON_H
#define TRIGON_H
#include "trigon/core/std/std.h"

typedef struct {
	cstr_t		app_name;
	version_t	app_version;
} app_info_t;

extern app_info_t app_main();
#endif // !TRIGON_H
