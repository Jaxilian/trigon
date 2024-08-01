#include "trigon/core/std/std.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_STR_ARG_LEN 1024

str_t::str_t(cstr_t txt, ...) {
	if (!txt) return;

	va_list args;
	va_start(args, txt);

	size_t size = strlen(txt) + MAX_STR_ARG_LEN + 1;
	buff = (char*)(malloc(size));
	if (!buff) return;

	vsnprintf(buff, size, txt, args);
	va_end(args);

	buff_len = (u32)strlen(buff);
	char* tmp = (char*)realloc(buff, buff_len + 1);
	if (!tmp) {
		free(buff);
		buff = nullptr;
		return;
	}

	buff = tmp;
	buff[buff_len] = '\0';

}

str_t::~str_t() {
	if (buff) {
		free(buff);
	}
}

void str_t::log() {
	if (!buff) {
		printf("NULL\n");
		return;
	}

	printf("%s\n", buff);
}


cstr_t str_t::cstr(){
	return buff;
}


