#pragma once
#include "trigon/core/util/defines.h"


class entity_t {
public:
	void set_parent(entity_t* entity);
	entity_t& parent() { return *local_parent; }

private:
	entity_t*	local_parent	= nullptr;
	guid_t		guid			= {};

public:
	entity_t(entity_t* new_parent = nullptr);
	~entity_t();
	entity_t(const entity_t&)				= delete;
	entity_t& operator=(const entity_t&)	= delete;
};