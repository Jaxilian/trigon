#ifndef TRIGON_ENTITY_H
#define TRIGON_ENTITY_H

#include <inttypes.h>
#include <stdbool.h>

#define ENTITY_TAG_LEN 32
#define ENTITY_MAX_CHILD_COUNT 31

typedef enum {
	CMP_TRANSFORM	= 0x01,
	CMP_MESH		= 0x02,
	CMP_COUNT		
} CMP_ENUM;

typedef struct {
	char		tag[ENTITY_TAG_LEN];
	void*		children[ENTITY_MAX_CHILD_COUNT];
	void*		parent;
	uint64_t	components;
} entity_t;

void entity_new(entity_t* entity, const char* tag);
bool entity_has_component(entity_t* entity, CMP_ENUM cmp);

#endif // !TRIGON_ENTITY_H
