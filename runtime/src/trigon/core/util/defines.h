#pragma once
#include <inttypes.h>
#include <string>

typedef const char* cstr_t;
typedef std::string	str_t;
typedef char guid_t[64];
typedef char path_t[255];

struct version_t {
	uint32_t version	= 1;
	uint32_t major		= 0;
	uint32_t minor		= 0;
	uint32_t patch		= 0;

	uint32_t merged		= 0;
	char	 str[64]	= {};

	cstr_t string();

	void compile() {
		merged = ((((uint32_t)(version)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)));
		string();
	}

	version_t() {};
	version_t(
		uint32_t _version,
		uint32_t _major,
		uint32_t _minor,
		uint32_t _patch
	) : version(_version), major(_major), minor(_minor), patch(_patch) {
		compile();
	};
};

void guid_gen(guid_t& id);