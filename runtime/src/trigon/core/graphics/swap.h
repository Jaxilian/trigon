#pragma once

class swapchain_t  {
public:
	void create(void* surface);
	swapchain_t()	{};
	~swapchain_t()	{};

private:
	void* _surface = nullptr;

};