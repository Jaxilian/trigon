#include "dev/std/std.h"
#include "dev/cpu/threads.h"
#include "dev/mem/mem.h"
#include "dev/cpu/coro.h"
#include "examples.h"
#include "dev/mem/core_ctx_def.h"
#include "dev/io/io.h"
#include "dev/win/win.h"
#include "dev/gpu/vk.h"


int main(int argc, char* argv[]) {
	core_ctx_new();

	path_new_root(argv[0]);

	win_new();
	vk_new();

	run_examples();

	while (ctx->win.running) {
		win_poll_events();
	}

	vk_del();
	win_del();
	core_ctx_del();

	mem_check();
	return 0;
}