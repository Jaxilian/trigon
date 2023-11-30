#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "dev/std/std.h"
#include "dev/cpu/threads.h"
#include "dev/mem/mem.h"
#include "dev/cpu/coro.h"
#include "examples.h"
#include "dev/mem/core_ctx_def.h"
#include "dev/io/io.h"
#include "dev/win/win.h"
#include "dev/gpu/vk.h"
#include <memory.h>


int common_main(int argc, char* argv[]) {
	core_ctx_new();

	path_new_root(argv[0]);

	win_new();
	vk_new();

	start_examples();

	while (ctx->win.running) {
		win_poll_events();
		vk_frame_begin();
        run_examples();
		vk_frame_end();
	}

    stop_examples();
	vk_del();
	win_del();
	core_ctx_del();

	mem_check();
	return 0;
}









#ifdef _DEBUG

int main(int argc, char* argv[]) {
    common_main(argc, argv);
    return 0;
}
#else

#ifdef LINUX

int main(int argc, char* argv[]) {
    common_main(argc, argv);
    return 0;
}

#else
#define SAFE_ARGV_SIZE 141
#pragma warning(disable : 28251)
#define MAX_ARGS 5

int _stdcall APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    char* argv[MAX_ARGS];
    int argc = 0;
    char buffer[MAX_PATH] = { 0 };

    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0) {
        argv[argc++] = _strdup(buffer);
    }
    else {
        return -1;
    }

    char* token = strtok(cmdline, " ");
    while (token && argc < MAX_ARGS) {
        argv[argc++] = _strdup(token);
        token = strtok(NULL, " ");
    }
    common_main(argc, argv);

    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    return 0;
}

#endif
#endif