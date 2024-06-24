#pragma once

class window_t {
public:
	 window_t();
	~window_t();

	void update();
	
	void* handle = nullptr;

private:
	static bool _initialized;

};