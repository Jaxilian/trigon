#include "std.h"
#include <stdlib.h>

cpool_t::cpool_t(size_t size, u32 predef_len) {
	stride	= size;
	max		= get_next_power_of_two(predef_len);

	queue	= (u32*)malloc(sizeof(u32) * max);
	data	= malloc(stride * (size_t)max);

	cassert(queue, "failed to allocate queue!\n");
	cassert(data, "failed to allocate data!\n");

	queue[0]	= 0;
	queue[1]	= 1;
	qiptr		= 0;
	qoptr		= 0;

	memset(data, 0, stride * max);
}
cpool_t::~cpool_t() {

}

vptr_t cpool_t::get(u32 idx) {

}

u32 cpool_t::add(vptr_t opt_data = nullptr) {

}

void cpool_t::pop(u32) {

}

void cpool_t::resize() {
	u32 new_max = max * 2;
	vptr_t new_data = malloc(stride * new_max);
	u32* new_queue = (u32*)malloc(sizeof(u32) * new_max);

	cassert(new_data, "failed to allocate queue!\n");
	cassert(new_queue, "failed to allocate data!\n");

	memset(new_data, 0, stride * new_max);

	memcpy(new_data, data, stride * max);
	memcpy(new_queue, queue, sizeof(u32) * max);

	for (u32 i = max; i < new_max; i++) {
		new_queue[i] = i;
	}

	free(data);
	free(queue);

	data = new_data;
	queue = new_queue;
	max = new_max;
}
