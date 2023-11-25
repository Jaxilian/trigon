#include "threads.h"

static void* threads[CPU_WORKER_COUNT] = { 0 };
static uint8_t active_workers = 0;

#ifdef _WIN32
#include <Windows.h>
static DWORD WINAPI win_thread_function(LPVOID lpParam) {
	event_t cb = (event_t)lpParam;
	cb();
	return 0;
}

static uint32_t win_get_cores() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

static uint32_t win_get_threads() {
	uint32_t cores = win_get_cores();
	uint32_t threads = cores * 2;
	return threads;
}

static void win_start_thread(event_t func, CPU_WORKER type) {
	uint32_t cores = win_get_threads();
	validate(cores >= CPU_WORKER_COUNT, "system does not have more or equal to %d cores", CPU_WORKER_COUNT);

	DWORD id;
	threads[type] = CreateThread(NULL, 0, win_thread_function, func, 0, &id);
	if (!threads[type]) {
		return;
	}
}

inline static void win_thread_await_all() {
	for (uint8_t i = 0; i < CPU_WORKER_COUNT; i++) {
		if (!active_workers & i) continue;
		if (threads[i] == NULL) continue;
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
		threads[i] = NULL;
	}
	active_workers = 0;
}

#endif


void thread_start(event_t function, CPU_WORKER type) {
	if (active_workers & type) {
		return;
	}
	active_workers |= type;
#ifdef _WIN32
	win_start_thread(function, type);
#endif
}

void thread_await_all() {
#ifdef _WIN32
	win_thread_await_all();
#endif
}
