#include "threads.h"

static void* threads[4]		= { 0 };
static uint8_t thread_count	= 0;

#ifdef _WIN32
#include <Windows.h>
static DWORD WINAPI win_thread_function(LPVOID lpParam) {
	event_t cb = (event_t)lpParam;
	cb();
	return 0;
}

inline static uint32_t win_get_cores() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

inline static void win_start_thread(event_t func) {
	uint32_t cores = win_get_cores();
	validate(cores >= 4, "system does not have more or equal to 4 cores");

	if (thread_count >= 4) {
		return;
	}

	DWORD id;
	threads[thread_count] = CreateThread(NULL, 0, win_thread_function, func, 0, &id);
	if (!threads[thread_count]) {
		return;
	}

	thread_count++;
}

inline static void win_thread_await_all() {
	for (uint8_t i = 0; i < thread_count; i++) {
		if (threads[i] == NULL) continue;
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
		threads[i] = NULL;
	}

	thread_count = 0;
}

#endif


void thread_start(event_t function) {
#ifdef _WIN32
	win_start_thread(function);
#endif
}

void thread_await_all() {
#ifdef _WIN32
	win_thread_await_all();
#endif
}
