#include "trigon.h"
#include "trigon/ecs/entitiy.h"

void trigon_setup_events() {
	entity_t player = { 0 };
	entity_new(&player, "hello");

	if (!entity_has_component(&player, CMP_TRANSFORM)) {

	}
}