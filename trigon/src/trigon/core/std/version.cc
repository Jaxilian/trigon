#include "std.h"
#include <stdio.h>
#include <string.h>

version_t::version_t(u32 version, u32 major, u32 minor, u32 patch)
	: _version(version), _major(major), _minor(minor), _patch(patch) {
	snprintf(
		_text,
		16,
		"%u.%u.%u.%u",
		_version,
		_major,
		_minor,
		_patch
	);
};

void version_t::set(u32 version, u32 major, u32 minor, u32 patch){
	_version	= version;
	_major		= major;
	_minor		= minor;
	_patch		= patch;

	snprintf(
		_text,
		16,
		"%u.%u.%u.%u",
		_version,
		_major,
		_minor,
		_patch
	);
};

cstr_t version_t::str() const {
	return _text;
}

cstr_t version_t::str() {
	snprintf(
		_text,
		16,
		"%u.%u.%u.%u",
		_version,
		_major,
		_minor,
		_patch
	);

	return _text;
}