#include "time.h"
#include <GLFW/glfw3.h>
#include "dev/mem/core_ctx_def.h"

void tick_update() {
	ctx->tick_pre = ctx->tick_now;
	ctx->tick_now =  glfwGetTime();
	ctx->tick_dt = ctx->tick_now - ctx->tick_pre;
}

inline
double time_dt() {
	return ctx->tick_dt;
}