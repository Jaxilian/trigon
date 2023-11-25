#include "core_ctx_def.h"
#include "mem.h"
#include <memory.h>

static core_ctx_t* ctx_cache = NULL;

void core_ctx_new() {
	validate(ctx_cache == NULL, "core_ctx was already allocated");
	ctx_cache = mem_alloc(sizeof(core_ctx_t));
	memset(ctx_cache, 0, sizeof(core_ctx_t));
	debug_log("allocated %zu bytes to core_ctx ", sizeof(core_ctx_t));
	debug_log_col(LOG_COL_GREEN, " ok\n");
}

inline core_ctx_t* core_ctx_get() {
	return ctx_cache;
}

void core_ctx_del() {
	mem_free(ctx_cache);
}