#ifndef T_SHADER_H
#define T_SHADER_H
#include "dev/std/std.h"
#include "dev/gpu/vk_cmn.h"

typedef struct {
	uint32_t			binding;
	shader_property_e	type;
	uint32_t			count;
} shader_property_value_t;

typedef struct {
	uint32_t				properties_count;
	shader_property_value_t	properties[MAX_DESC_PER_SET];
} shader_property_t;

typedef struct {
	bool x; //TODO: continue
} shader_property_info_t;

void shader_property_new();
void shader_property_del();



typedef struct {
	cstr					shader_name;
	uint32_t				property_sets_count;
	shader_property_t		property_sets[MAX_DESCRIPTOR_SETS_IN_USE];
} shader_info_t;

typedef struct {
	cstr					name;
	shader_property_t		property_sets[MAX_DESCRIPTOR_SETS_IN_USE];
} shader_t;

void shader_new(shader_info_t* info);
void shader_del();


#endif