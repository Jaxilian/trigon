#include "vkl.h"
#include "stdio.h"
#include "stdlib.h"

_Noreturn void vkl_error(const char* msg, vkl_error_enum type) {
	printf("%s\n", msg);
	if (type == ERROR_FATAL) {
		exit(1);
	}
}