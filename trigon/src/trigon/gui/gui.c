#include "gui.h"
#include "cglm/vec2.h"

typedef struct {
	vec2 pos;
	vec2 size;
} sprite_t;

static b8		initialized = false;
static cpool_t	pool		= { 0 };

static inline void init() {
	if (initialized) return;
	cpool_new(&pool, sizeof(sprite_t), 8);
	initialized = true;
}

u32 gui_new() {
	init();

	return cpool_add(&pool, NULL);
}

void gui_del(u32 id) {
	if (!initialized) return;

	cpool_pop(&pool, id);
	
	if (&pool.length <= 0) {
		cpool_del(&pool);
		initialized = false;
	}
}