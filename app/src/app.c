#include "trigon.h"
#include "trigon/ecs/entitiy.h"
#include "trigon/cam/camera.h"

static camera_t camera = { 0 };

void trigon_setup_events() {

	camera_new(&camera, 45.0f, 0.01f, 1000.0f);

	entity_t player = { 0 };
	entity_new(&player, "hello");

	if (!entity_has_component(&player, CMP_TRANSFORM)) {

	}
}