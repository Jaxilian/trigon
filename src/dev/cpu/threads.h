#ifndef T_THREADS_H
#define T_THREADS_H
#include "dev/std/std.h"


typedef enum {
	CPU_WORKER_REND,
	CPU_WORKER_CORO,
	CPU_WORKER_COUNT
} CPU_WORKER;

typedef struct {
	bool locked;
} mutex_t;

void thread_start(event_t function, CPU_WORKER type);
void thread_await_all();

#endif // !T_THREADS_H
