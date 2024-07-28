#include "cstd/cstd.h"
#include "tgcore/win/win.h"



int main() {
	debug_log_clear();
	service_win_start();

	service_win_stop();

	return 0;
}