#include "dev/std/std.h"
#include "dev/cpu/threads.h"
#include "dev/mem/mem.h"
#include "dev/io/io.h"

int main(int argc, char* argv[]) {
	paths_new(argv[0]);
	debug_log("%s\n", paths_get("Test!"));
	debug_log("%s\n", paths_get("Test3"));
	paths_del();
	mem_check();
	return 0;
}