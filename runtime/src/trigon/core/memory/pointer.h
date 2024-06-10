#pragma once
#include <inttypes.h>

template <typename T>
class ptr_t {
private:
	T* ptr = nullptr;
	uint32_t len;

public:
	ptr_t(uint32_t count = 1) {
		len = count;
		if (len > 1) {
			ptr = new T[len];
		}
		else {
			ptr = new T();
		}
	}

	~ptr_t() {
		if (len > 1) {
			delete[] ptr;
		}
		else {
			delete ptr;
		}
	}

	void restart() {
		if (len > 1) {
			delete[] ptr;
			ptr = new T[len];
		}
		else {
			delete ptr;
			ptr = new T();
		}
	}

	T& get() {
		return *ptr;
	}
};