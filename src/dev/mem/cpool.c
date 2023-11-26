#include "mem.h"


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
	void* new_data		= mem_alloc(pool->stride * new_max);
	uint32_t* new_queue = mem_alloc(sizeof(uint32_t) * new_max);

	validate(new_data, "failed to allocate queue!\n");
	validate(new_queue, "failed to allocate data!\n");

	memset(new_data, 0, pool->stride * new_max);

	memcpy(new_data, pool->data, pool->stride * pool->max_count);
	memcpy(new_queue, pool->queue, sizeof(uint32_t) * pool->max_count);

	for (uint32_t i = pool->max_count; i < new_max; i++) {
		new_queue[i] = i;
	}

	mem_free(pool->data);
	mem_free(pool->queue);

	pool->data = new_data;
	pool->queue = new_queue;
	pool->max_count = new_max;
}

void cpool_new(cpool_t* pool, size_t stride, uint32_t preset_length) {
	uint32_t predef_len = get_next_power_of_two(preset_length);

	pool->stride = stride;
	pool->max_count = predef_len;
	pool->length = 0;
	pool->queue = mem_alloc(sizeof(uint32_t) * pool->max_count);
	pool->data = mem_alloc(stride * (size_t)pool->max_count);

	validate(pool->queue, "failed to allocate queue!\n");
	validate(pool->data, "failed to allocate data!\n");

	pool->queue[0] = 0;
	pool->queue[1] = 1;

	pool->queue_in_ptr = 0;
	pool->queue_out_ptr = 0;

	memset(pool->data, 0, pool->stride * pool->max_count);
}

void cpool_del(cpool_t* pool) {
	mem_free(pool->queue);
	mem_free(pool->data);
	memset(pool, 0, sizeof(*pool));
	pool->queue = NULL;
	pool->data = NULL;
}

void* cpool_get(cpool_t* pool, uint32_t index) {
	return (void*)((char*)pool->data + index * pool->stride);
}

uint32_t cpool_add(cpool_t* pool, void* data) {
	if (pool->length >= pool->max_count) {
		pool->queue_out_ptr = pool->max_count;
		cpool_resize(pool);
	}

	uint32_t empty_slot = pool->queue[pool->queue_out_ptr];

	if (data != NULL) {
		void* slot_ptr = cpool_get(pool, empty_slot);
		memcpy(slot_ptr, data, pool->stride);
	}
	else {
		void* slot_ptr = cpool_get(pool, empty_slot);
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
	validate(pool->length > 0, "pool is already empty!\n");

	pool->queue[pool->queue_in_ptr] = index;
	pool->queue_in_ptr = (pool->queue_in_ptr + 1) % pool->max_count;
	pool->length--;

	void* ptr = (void*)((char*)pool->data + index * pool->stride);
	memset(ptr, 0, pool->stride);
}

size_t cpool_mem(cpool_t* pool) {
	return pool->stride * pool->max_count;
}