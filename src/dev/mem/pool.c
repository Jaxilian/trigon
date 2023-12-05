#include "pool.h"
#include "mem.h"

bool mem_pool_new(size_t stride,uint32_t count, mem_pool_t* out) {
	memset(out, 0, sizeof(mem_pool_t));

	size_t total_size = stride * count;
	out->count	= count;
	out->stride = stride;
	out->size	= total_size;

	out->data = mem_alloc(total_size);
	return true;
}

inline
void* pool_get(mem_pool_t* in, size_t index) {
	return (void*)((char*)in->data + index * in->stride);
}

void pool_del(mem_pool_t* pool) {
	if (pool->data) {
		mem_free(pool->data);
	}
	memset(pool, 0, sizeof(mem_pool_t));
}

void pool_set(mem_pool_t* in, size_t index, void* data) {
	void* ptr = pool_get(in, index);
	memcpy(ptr,data, in->stride);
}

void pool_pop(mem_pool_t* in, size_t index) {
	void* ptr = pool_get(in, index);
	memset(ptr, 0, in->stride);
}