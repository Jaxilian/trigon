#include "managers.h"
#include "shadman.h"

void managers_new() {
	shader_simple_new();
	
}

void managers_on_draw() {
	shader_simple_draw(0);
}

void managers_del() {
	shader_simple_del();
}

void managers_on_win_resize() {
	shader_simple_new();
}