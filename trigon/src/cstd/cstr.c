#include "cstd.h"

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void cstr_to_wchar(cstr_t str, wchar_t* wchar) {
	size_t len = mbstowcs(NULL, str, 0);
	if (len == (size_t)(-1)) return;
	mbstowcs(wchar, str, len + 1);
}

void wchar_to_cstr(const wchar_t* wchar, char* str) {
	size_t len = wcstombs(NULL, wchar, 0);
	if (len == (size_t)(-1)) return;
	wcstombs(str, wchar, len + 1);
}

void version_to_str(const version_t* v, str64_t str) {
	snprintf(
		str, 
		sizeof(64),
		"%u.%u.%u.%u",
		v->version,
		v->major,
		v->minor,
		v->patch
	);
}

uint32_t cstr_split(cstr_t in, cstr_t delimiter, char** out) {
	char* str_cpy = strdup(in);

	if (str_cpy == NULL) {
		printf("failed to allocate memory for copy\n");
		return false;
	}

	char* token = strtok(str_cpy, delimiter);

	int buffer_index = 0;
	while (token != NULL) {

		if (out) {
			strcpy(out[buffer_index], token);
		}

		buffer_index++;
		token = strtok(NULL, (const char*)delimiter);
	}

	free(str_cpy);
	return buffer_index;

}

