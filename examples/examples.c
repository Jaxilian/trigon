#include "examples.h"
#include "dev/win/win.h"
#include "dev/time/time.h"
#include <stdio.h>

void start_examples() {

}


void run_examples() {

	double dt = time_dt();
	uint32_t fps = (uint32_t)(60.0 / dt);
	char name_buffer[25] = { 0 };
	sprintf_s(name_buffer, 25, "fps: %d", fps);
	win_set_name(name_buffer);

}

void stop_examples() {
	

}