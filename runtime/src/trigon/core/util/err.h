#pragma once
#include "defines.h"


class debug_t {
public:
	// log error message
	[[noreturn]] static void err(cstr_t format, ...);

	//log warning message
	static void wrn(cstr_t format, ...);

	//log message
	static void log(cstr_t format, ...);

private:
	 debug_t();
	 ~debug_t() {};

public: //singleton
	debug_t(const debug_t&) = delete;
	debug_t& operator=(const debug_t&) = delete;
	static inline debug_t& get() {
		static debug_t instance;
		return instance;
	}
};
