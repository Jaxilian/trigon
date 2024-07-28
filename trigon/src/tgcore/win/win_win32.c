#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include "win.h"




#ifdef _DEBUG

static cstr_t extensions[] = { 
	"VK_KHR_surface",
	"VK_KHR_win32_surface",
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME 
};
static uint32_t extension_count = 3;

#else
static cstr_t	extensions[] = { "VK_KHR_surface","VK_KHR_win32_surface" };
static uint32_t extension_count = 2;
#endif

typedef struct {
	uint32_t		id;
	vec2			extent;
	vec2			position;
	float			aspect;
	vptr_t			handle;
	cstr_t			name;
	VkSurfaceKHR	surface;
} win_data_t;

static win_data_t main_window = { 0 };

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_CLOSE:
		//app_quit();
		return 0;
	case WM_DWMWINDOWMAXIMIZEDCHANGE:
		//sync_resize(hwnd);
		break;
	case WM_SIZE:
		//sync_resize(hwnd);
	case WM_EXITSIZEMOVE:
		//sync_resize(hwnd);
		break;
	case WM_DESTROY:
		//app_quit();
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

uint32_t win_new(cstr_t name) {
	if (main_window.handle != NULL) {
		return main_window.id;
	}

	main_window.name	= name;
	main_window.position[0] = CW_USEDEFAULT;
	main_window.position[1] = CW_USEDEFAULT;
	main_window.extent[0] = 800;
	main_window.extent[1] = 600;
	main_window.aspect = 800.0f / 600.0f;

	HINSTANCE hinstance = GetModuleHandle(NULL);
	WNDCLASSEXW wc = { 0 };
	HWND hwnd;

	wchar_t buffer[260] = { 0 };
	cstr_to_wchar(name, buffer);

	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = 0;
	wc.lpfnWndProc = wnd_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"trigon_win_t";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassExW(&wc)) {
		debug_err("failed to create window for WIN32");
		return 0;
	}

	hwnd = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		wc.lpszClassName,
		buffer,
		WS_OVERLAPPEDWINDOW,
		(int)main_window.position[0],
		(int)main_window.position[1],
		(int)main_window.extent[0],
		(int)main_window.extent[1],
		NULL, NULL, hinstance, NULL
	);

	if (!hwnd) {
		debug_err("failed to create window for WIN32");
		return 0;
	}

	ShowWindow(hwnd, SW_SHOW);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&main_window);
	main_window.handle = hwnd;

	return main_window.id;
}

cstr_t* win_extensions(uint32_t* _extc) {
	*_extc = extension_count;
	return extensions;
}

void win_del(uint32_t id) {
	if (!main_window.handle) return;

	DestroyWindow((HWND)main_window.handle);
	memset(&main_window, 0, sizeof(win_data_t));
}

void win_get_extent(uint32_t id, vec2 out) {
	glm_vec2_copy(
		main_window.extent,
		out
	);
}

void win_get_position(uint32_t id, vec2 out) {
	glm_vec2_copy(
		main_window.position,
		out
	);
}

float win_get_aspect(uint32_t id) {
	return main_window.aspect;
	return 0;
}

vptr_t win_get_handle(uint32_t id) {
	return main_window.handle;
}

void service_win_start() {
	win_new("trigon");
}
void service_win_stop() {
	win_del(0);
}

VkSurfaceKHR win_surface(uint32_t id) {
	if (!main_window.surface) {
		if (main_window.handle) {
			/*
			vkCreateWin32SurfaceKHR(
				rendr_get()->device.instance,
				&info, NULL,
				&ptr->surface
			);
			*/
		}
		else {
			return NULL;
		}
	}

	return main_window.surface;
}

#endif