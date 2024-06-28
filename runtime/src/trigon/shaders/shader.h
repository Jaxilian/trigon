#pragma once

#include <inttypes.h>
#include <vulkan/vulkan.h>
#include <vector>

enum SHADER_STAGE {
	fragment,
	vertex,
};

enum SHADER_PROP {
	static_buffer,
	dynamic_buffer,
	sampler_buffer,
};

class shader_sets_t {
public:
	void add(uint32_t location, SHADER_PROP property, uint32_t count = 1);
	void bind();

	struct shader_set_t {
		bool		initialized;
		uint32_t	location;
		SHADER_PROP property;
		uint32_t	count;
	};

	std::vector<shader_set_t> sets;
private:
	bool bound = false;
};

class shader_t {
public:
	void add_resource(uint32_t location, SHADER_PROP property, uint32_t count = 1);
	void bind_resources();

private:

	shader_sets_t sets;

	bool bound = false;
};