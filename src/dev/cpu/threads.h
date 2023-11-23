#ifndef T_THREADS_H
#define T_THREADS_H
#include "dev/std/std.h"

typedef struct {
	bool locked;
} mutex_t;

void thread_start(event_t function);
void thread_await_all();

#endif // !T_THREADS_H
