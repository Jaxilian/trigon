#pragma once
#include "trigon/core/std/std.h"

class shader_t {
public:
	shader_t() {};
	shader_t(cstr_t name, cstr_t path);
	~shader_t();

	shader_t(const shader_t&) = delete;
	shader_t& operator=(const shader_t&) = delete;

	void compile();


private:
	cstr_t  _name			= NULL;
	cstr_t	_shader_path	= NULL;
	bool	_compiled		= false;

};