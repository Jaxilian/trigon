#include "cstd.h"
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void version_to_str(const version_t* v, char str[64]) {
	snprintf(str, sizeof(64), "%u.%u.%u.%u", v->version, v->major, v->minor, v->patch);
}

bool cstr_to_wchar(const char* str, wchar_t* wchar) {
#ifdef _WIN32
	size_t len = 0;
	errno_t err = mbstowcs_s(&len, NULL, 0, str, _TRUNCATE);
	if (err != 0 || len == 0) {
		return false;
	}

	size_t converted = 0;
	err = mbstowcs_s(&converted, wchar, len, str, _TRUNCATE);
	if (err != 0 || converted == 0) {
		return false;
	}
#else
	size_t len = mbstowcs(NULL, str, 0);
	if (len == (size_t)(-1)) return false;
	mbstowcs(wchar, str, len + 1);
#endif

	return true;
}

bool wchar_to_cstr(const wchar_t* wchar, char* str) {
#ifdef _WIN32
	size_t len = 0;
	errno_t err = wcstombs_s(&len, NULL, 0, wchar, _TRUNCATE);

	if (err != 0 || len == 0) {
		return false;
	}

	size_t converted = 0;
	err = wcstombs_s(&converted, str, len, wchar, _TRUNCATE);
	if (err != 0 || converted == 0) {
		return false;
	}
#else
	size_t len = wcstombs(NULL, wchar, 0);
	if (len == (size_t)(-1)) return false;
	wcstombs(str, wchar, len + 1);
#endif

	return true;
}

bool string_split(const char* in, const char* delimiter, char** out, int out_len) {
#ifdef _WIN32
	char* str_cpy = _strdup(in);
#else
	char* str_cpy = strdup(in);
#endif
	if (str_cpy == NULL) {
		printf("failed to allocate memory for copy\n");
		return false;
	}

	char* token = strtok(str_cpy, delimiter);

	int buffer_index = 0;
	while (token != NULL) {
		cassert(buffer_index < out_len, "buffer overflown!\n");

		strcpy(out[buffer_index], token);
		buffer_index++;
		token = strtok(NULL, (const char*)delimiter);
	}

	free(str_cpy);
	return true;
}