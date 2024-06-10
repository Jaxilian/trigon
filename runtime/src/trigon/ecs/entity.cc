#include "entity.h"

entity_t::entity_t(entity_t* new_parent) {
	guid_gen(guid);
}

entity_t::~entity_t() {

}

void entity_t::set_parent(entity_t* entity) {
	local_parent = entity;
}