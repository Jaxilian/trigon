#include "cstd.h"
#include <stdlib.h>
#include <memory.h>

vptr_t cpool_get(cpool_t* pool, uint32_t index) {
	return (vptr_t)((char*)pool->data + index * pool->stride);
}

static uint32_t get_next_power_of_two(uint32_t number) {
	if (number <= 2) {
		return 2;
	}

	uint32_t num = 2;
	while (number > num) {
		num *= 2;
	}
	return num;
}

static void cpool_resize(cpool_t* pool) {
	uint32_t new_max	= pool->max_count * 2;
	vptr_t new_data		= malloc(pool->stride * new_max);
	uint32_t* new_queue = (uint32_t*)malloc(sizeof(uint32_t) * new_max);

	cassert(new_data, "failed to allocate queue!\n");
	cassert(new_queue, "failed to allocate data!\n");

	memset(new_data, 0, pool->stride * new_max);

	memcpy(new_data, pool->data, pool->stride * pool->max_count);
	memcpy(new_queue, pool->queue, sizeof(uint32_t) * pool->max_count);

	for (uint32_t i = pool->max_count; i < new_max; i++) {
		new_queue[i] = i;
	}

	free(pool->data);
	free(pool->queue);

	pool->data = new_data;
	pool->queue = new_queue;
	pool->max_count = new_max;
}

void cpool_new(cpool_t* pool, size_t stride, uint32_t preset_length) {
	uint32_t predef_len = get_next_power_of_two(preset_length);

	pool->stride = stride;
	pool->max_count = predef_len;
	pool->length = 0;
	pool->queue = (uint32_t*)malloc(sizeof(uint32_t) * pool->max_count);
	pool->data = malloc(stride * (size_t)pool->max_count);

	cassert(pool->queue, "failed to allocate queue!\n");
	cassert(pool->data, "failed to allocate data!\n");

	pool->queue[0] = 0;
	pool->queue[1] = 1;

	pool->queue_in_ptr = 0;
	pool->queue_out_ptr = 0;

	memset(pool->data, 0, pool->stride * pool->max_count);
}

void cpool_del(cpool_t* pool) {
	free(pool->queue);
	free(pool->data);
	memset(pool, 0, sizeof(*pool));
	pool->queue = NULL;
	pool->data = NULL;
}


uint32_t cpool_add(cpool_t* pool, vptr_t data) {
	if (pool->length >= pool->max_count) {
		pool->queue_out_ptr = pool->max_count;
		cpool_resize(pool);
	}

	uint32_t empty_slot = pool->queue[pool->queue_out_ptr];

	if (data != NULL) {
		vptr_t slot_ptr = cpool_get(pool, empty_slot);
		memcpy(slot_ptr, data, pool->stride);
	}
	else {
		vptr_t slot_ptr = cpool_get(pool, empty_slot);
		memset(slot_ptr, 0, pool->stride);
	}

	pool->queue_out_ptr = (pool->queue_out_ptr + 1) % pool->max_count;
	pool->length++;

	return empty_slot;
}

uint32_t cpool_len(cpool_t* pool) {
	return pool->length;
}

void cpool_pop(cpool_t* pool, uint32_t index) {
	cassert(pool->length > 0, "pool is already empty!\n");

	pool->queue[pool->queue_in_ptr] = index;
	pool->queue_in_ptr = (pool->queue_in_ptr + 1) % pool->max_count;
	pool->length--;

	vptr_t ptr = (vptr_t)((char*)pool->data + index * pool->stride);
	memset(ptr, 0, pool->stride);
}

size_t cpool_mem(cpool_t* pool) {
	return pool->stride * pool->max_count;
}