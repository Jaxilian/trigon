#ifndef T_SHADER_H
#define T_SHADER_H
#include "dev/std/std.h"
#include "dev/gpu/vk_cmn.h"

typedef struct {
	uint32_t			binding;
	shader_property_e	type;
	uint32_t			count;
	size_t				data_count;
	size_t				data_stride;
	uint32_t			data_width;
	uint32_t			data_height;
	uint32_t			data_channels;
} shader_resource_info_t;

typedef struct {
	uint32_t			binding;
	shader_property_e	type;
	uint32_t			count;
	uint32_t			desc_id;
	void*				data;
	size_t				data_count;
	size_t				data_stride;
	uint32_t			data_width;
	uint32_t			data_height;
	uint32_t			data_channels;
} shader_resource_t;

typedef struct {
	uint32_t			resource_count;
	shader_resource_t	resources[MAX_DESC_PER_SET];
} shader_resource_group_info_t;

typedef struct {
	uint32_t			desc_set_id;
	uint32_t			resource_count;
	shader_resource_t	resources[MAX_DESC_PER_SET];
	bool				has_dynamic_buffer;
} shader_resource_group_t;

void shader_resource_new(shader_resource_info_t* in, shader_resource_t* out);
void shader_resource_del(shader_resource_t* in);
void shader_resource_set(shader_resource_t* resource, void* data);

void shader_resource_group_new(
	shader_resource_group_info_t* info,
	shader_resource_group_t* out
);

void shader_resource_group_del(shader_resource_group_t* out);

typedef struct {
	cstr					 shader_name;
	uint32_t				 group_count;
	shader_resource_group_t* groups[MAX_DESC_SETS_IN_USE];
} shader_info_t;

typedef struct {
	cstr						name;
	uint32_t					pipeline;
	uint32_t					group_count;
	shader_resource_group_t*	groups[MAX_DESC_SETS_IN_USE];
} shader_t;

void shader_new(shader_info_t* info, shader_t* out);
void shader_use(shader_t* shader);
void shader_del(shader_t* shader);


#endif