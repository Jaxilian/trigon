#ifndef TG_APPLICATION_H
#define TG_APPLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "trigon/core/utils/util.h"

typedef struct {
	cstr_t		name;
	version_t	version;
	voidcb_t	on_start;
	voidcb_t	on_update;
	voidcb_t	on_quit;
} app_info_t;

void app_load(app_info_t* info);
void app_quit();


extern inline bool		app_quitting();
extern inline cstr_t	app_name();
extern inline uint32_t	app_version();
extern inline uint32_t	engine_version();
extern inline cstr_t	engine_name();

#ifdef __cplusplus
}
#endif


#endif // !APPLICATION_H
