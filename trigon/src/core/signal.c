#include "signal.h"
#include "vstd/cpool.h"

typedef struct {
	signal_cb cb;
	uint32_t  type;
} signal_t;

static cpool_t signal_pools[SIGNAL_TYPE_COUNT] = {0};


uint32_t signal_connect(SIGNAL_TYPE _type, signal_cb _cb) {
	signal_t data = {
		.cb = _cb,
		.type = _type,
	};

	uint32_t id = cpool_add(&signal_pools[(uint32_t)_type], &data);
	return id;
}

void signal_disconnect(SIGNAL_TYPE type, uint32_t id) {
	cpool_pop(&signal_pools[type], id);
}

void signal_fire(SIGNAL_TYPE type) {
	cpool_t* pool = &signal_pools[(uint32_t)type];

	for (uint32_t i = 0; i < pool->max_count; i++) {
		signal_t* signal = (signal_t*)cpool_get(pool, i);
		if (signal->cb == NULL) continue;
		signal->cb();
	}
}

void signal_init() {
	for (uint32_t i = 0; i < SIGNAL_TYPE_COUNT; i++) {
		cpool_new(&signal_pools[i], sizeof(signal_cb), 8);
	}
}


void signal_clear() {
	for (uint32_t i = 0; i < SIGNAL_TYPE_COUNT; i++) {
		cpool_del(&signal_pools[i]);
	}
}