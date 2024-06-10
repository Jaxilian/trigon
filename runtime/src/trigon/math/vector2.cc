#include "vector2.h"
#include <glm/vec2.hpp>

vector2_t& vector2_t::set(float x, float y) {
	data[0] = x;
	data[1] = y;

	return *this;
}

vector2_t::vector2_t(float x, float y) {
	set(x, y);
}

const float& vector2_t::x() {
	return data[0];
}

const float& vector2_t::y() {
	return data[1];
}

