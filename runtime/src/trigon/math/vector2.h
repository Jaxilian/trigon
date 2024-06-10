#pragma once

class vector2_t {
public:
	vector2_t(float x = 0, float y = 0);
	vector2_t& set(float x, float y);

	const float& x();
	const float& y();

	vector2_t& operator=(vector2_t& rhs) {
		set(rhs.x(), rhs.y());
		return *this;
	}

private:
	float data[2];
};