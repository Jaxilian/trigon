#ifndef TG_SERVICES_H
#define TG_SERVICES_H
#include "cstd/cstd.h"

typedef enum {
	SERVICE_NONE,
	SERVICE_RENDERER,
	SERVICE_COUNT
} SERVICE;

typedef enum {
	SERVICE_STATE_NONE,
	SERVICE_STATE_START,
	SERVICE_STATE_STOP,
	SERVICE_STATE_UPDATE,
	SERVICE_STATE_RENDER,
	SERVICE_STATE_COUNT
} SERVICE_STATE;

typedef struct {
	cstr_t		name;
	vptrcb_t	event_cbs[SERVICE_STATE_COUNT];
} service_t;

typedef uint32_t SERVICE_FLAGS;

void services_set(SERVICE_FLAGS services, SERVICE_STATE state);
bool services_active(SERVICE service);


#endif // !TG_SERVICES_H
