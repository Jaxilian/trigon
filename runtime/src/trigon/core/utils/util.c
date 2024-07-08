#include "util.h"
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void version_to_str(const version_t* v, char str[64]) {
	snprintf(str, sizeof(str), "%u.%u.%u.%u", v->version, v->major, v->minor, v->patch);
}


bool string_split(const char* in, const char* delimiter, char** out, int out_len) {
	char* str_cpy = strdup(in);

	if (str_cpy == NULL) {
		printf("failed to allocate memory for copy\n");
		return false;
	}

	char* token = strtok(str_cpy, delimiter);

	int buffer_index = 0;
	while (token != NULL) {
		if (buffer_index >= out_len) {
			printf("buffer overflown!\n");
			return false;
		}

		strcpy(out[buffer_index], token);
		buffer_index++;
		token = strtok(NULL, (const char*)delimiter);
	}

	free(str_cpy);
	return true;
}
void title_new(title_t title, const char* string) {
	strcpy_s(title, sizeof(title_t), string);
}
