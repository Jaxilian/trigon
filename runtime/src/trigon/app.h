#pragma once
#include "trigon/core/util/err.h"

#ifdef _RUNTIME
#include "trigon/core/vulkan/vkrend.h"
#endif

struct app_config_t {
	cstr_t		app_name;
	version_t	version;
};

class app_t {
public:
	static void load(const app_config_t& conf);

	static version_t	engine_version;
	static app_config_t config;
	static bool			config_added;
	
	path_t  current_path	= {};
	path_t  app_path		= {};
	
#ifdef _RUNTIME
	vkrend_t& rend;
#endif

private:

	 app_t();
    ~app_t();





public: //singleton
	app_t(const app_t&) = delete;
	app_t& operator=(const app_t&) = delete;
	static inline app_t& get() {
		static app_t instance;
		return instance;
	}
};