#include "input.h"

#include "input.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include <memory.h>

static bool key_states[KEYCODE_COUNT] = { 0 };
static vec2 cursor, cursor_prev;
static bool mouse_initialized = false;
static bool mouse_locked = false;
static GLFWwindow* window;

static void input_key_sync(uint32_t prim_key, uint32_t sec_key, uint32_t action) {
	key_states[prim_key] = true;
}

static void window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}

static void window_mouse_callback(GLFWwindow* window, double x, double y) {

}

static void window_input_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input_key_sync(key, mods, action);
}



bool input_get_key(enum KEYCODE code) {
	return glfwGetKey(window, code);
}

bool input_key_clicked(enum KEYCODE code) {
	return key_states[code];
}

void input_reset() {
	memset(key_states, 0, sizeof(key_states));
}

void input_sync() {
	cursor_prev[0] = cursor[0];
	cursor_prev[1] = cursor[1];

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursor[0] = (float)x;
	cursor[1] = (float)y;

	if (!mouse_initialized) {
		cursor_prev[0] = cursor[0];
		cursor_prev[1] = cursor[1];
		glfwSetKeyCallback(window, window_input_cb);
		glfwSetCursorPosCallback(window, window_mouse_callback);
		glfwSetMouseButtonCallback(window, window_mouse_button_callback);
		mouse_initialized = true;
	}

	if (mouse_locked) {
		//glfwSetCursorPos(win_get(), 0, 0);
	}
}

void input_cursor_lock(bool value) {
	if (mouse_locked == value) return;

	mouse_locked = value;
	if (mouse_locked) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

bool input_mouse_clicked(int id) {
	return glfwGetMouseButton(window, id);
}

float input_mouse_x() {
	return (float)cursor[0];
}

float input_mouse_y() {
	return (float)cursor[1];
}

float input_mouse_prev_x() {
	return (float)cursor_prev[0];
}

float input_mouse_prev_y() {
	return (float)cursor_prev[1];
}

void  input_init(void* winptr) {
	window = winptr;
}
