#include "entitiy.h"
#include <memory.h>
#include <stdio.h>

void entity_new(entity_t* entity, const char* tag) {
	memset(entity, 0, sizeof(entity_t));

	if (tag == NULL) {
		printf("entities needs a tag!\n");
		return;
	}


}

bool entity_has_component(entity_t* entity, CMP_ENUM cmp) {
	return entity->components & cmp;
}