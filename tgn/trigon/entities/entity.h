#ifndef TGN_ENTITY_H
#define TGN_ENTITY_H

#include "trigon/math/math.h"

typedef struct {
	mat4		matrix;
	vec3		position;
	vec3		rotation;
	vec3		forward;
	vec3		up;
	vec3		right;
} transform_t;

typedef struct {
	char	id[32];

	transform_t transform;

	struct	entity_t* parent;
	struct	entity_t* children;

	uint32_t child_count;
	void*	 sub_struct;
} entity_t;


#endif