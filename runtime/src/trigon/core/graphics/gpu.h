#pragma once
#include <inttypes.h>

struct queue_family_t {
	uint32_t present_family		= 0;
	uint32_t graphics_family	= 0;
};

class gpu_t {
public:
	
	void bind_device(void* surface);

	void* instance	= nullptr;
	void* debugger	= nullptr;
	void* physical	= nullptr;
	void* cmd_pool  = nullptr;
	void* device	= nullptr;

	void* graphics_queue	= nullptr;
	void* present_queue		= nullptr;

	queue_family_t que_fam;


private:

	 gpu_t();
	~gpu_t();


	void create_instance();
	void get_physical_device(void* surface);

	bool device_suitable(void* physical_device, void* surface);
	void get_queue_families(
		void*				physical_device,
		bool&				success,
		uint32_t&			g_family,
		uint32_t&			p_family,
		void*				surface
	);



public: //singleton
	gpu_t(const gpu_t&) = delete;
	gpu_t& operator=(const gpu_t&) = delete;
	static inline gpu_t& get() {
		static gpu_t i;
		return i;
	}

};