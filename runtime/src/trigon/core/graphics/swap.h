#pragma once
#include "win.h"

class swapchain_t  {
public:
	swapchain_t(win_t& window);
	void refresh();
	~swapchain_t();

private:
	
	void create_swapchain();

	win_t& _window;
	void* _data;
};