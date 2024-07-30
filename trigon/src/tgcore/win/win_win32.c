#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include "win.h"
#include <vulkan/vulkan_win32.h>

#ifdef _DEBUG

static cstr_t extensions[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
static uint32_t extension_count = 3;

#else
static cstr_t extensions[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
static uint32_t extension_count = 2;
#endif

typedef struct {
    uint32_t    id;
    vec2        extent;
    vec2        position;
    float       aspect;
    cstr_t      name;
    HINSTANCE   hinstance;
    HWND        hwnd;

} win_data_t;

static win_data_t windows[1] = {
    [0] = {0}
};
static win_event_cb_t event_callback = NULL;

void win_event_callback(win_event_cb_t cb) {
    event_callback = cb;
}


static void sync_window(HWND hwnd) {
    win_data_t* ptr = (win_data_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!ptr) return;

    RECT rect;
    if (GetClientRect(hwnd, &rect)) {
        uint32_t width  = (uint32_t)(rect.right - rect.left);
        uint32_t height = (uint32_t)(rect.bottom - rect.top);

        if (width == ptr->extent[0] && height == ptr->extent[1]) return;

        ptr->extent[0] = (float)width;
        ptr->extent[1] = (float)height;
        ptr->aspect = (float)width / (float)height;

        if (
            ptr->extent[0] > 0 &&
            ptr->extent[1] > 0 &&
            event_callback) {
            event_callback(ptr->id, WIN_EVENT_RESIZE);
        }

    }
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    win_data_t* ptr = (win_data_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CLOSE:
        if(ptr) win_del(ptr->id);
        return 0;
    case WM_DWMWINDOWMAXIMIZEDCHANGE:
        sync_window(hwnd);
        break;
    case WM_SIZE:
        sync_window(hwnd);
    case WM_EXITSIZEMOVE:
        sync_window(hwnd);
        break;
    case WM_DESTROY:
        if (ptr) win_del(ptr->id);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void win_resize(uint32_t id, vec2 new_size) {
    if (!windows[id].hwnd) return;
    
    glm_vec2_copy(new_size, windows->extent);
    SetWindowPos(
        windows[id].hwnd, NULL,
        (int)windows[id].position[0],   (int)windows[id].position[1],
        (int)windows[id].extent[0],     (int)windows[id].extent[1],
        SWP_NOZORDER
    );

    float aspect = (float)windows[id].extent[0] / (float)windows[id].extent[1];
    windows[id].aspect = aspect;
}

uint32_t win_new(cstr_t name) {
    uint32_t HARDCODED_WINDOW = 0;
    uint32_t id = HARDCODED_WINDOW;

    if (windows[id].hwnd != NULL) {
        return id;
    }

    windows[id].name = name;
    windows[id].position[0] = CW_USEDEFAULT;
    windows[id].position[1] = CW_USEDEFAULT;
    windows[id].extent[0] = 800;
    windows[id].extent[1] = 600;
    windows[id].aspect = 800.0f / 600.0f;
    windows[id].id = id;

    windows[id].hinstance = GetModuleHandle(NULL);
    WNDCLASSEXW wc = { 0 };

    wchar_t buffer[260] = { 0 };
    cstr_to_wchar(name, buffer);

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = windows[id].hinstance;
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

    windows[id].hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        buffer,
        WS_OVERLAPPEDWINDOW,
        (int)windows[id].position[0],
        (int)windows[id].position[1],
        (int)windows[id].extent[0],
        (int)windows[id].extent[1],
        NULL, NULL, windows[id].hinstance, NULL
    );

    if (!windows[id].hwnd) {
        debug_err("failed to create window for WIN32");
        return 0;
    }

    ShowWindow(windows[id].hwnd, SW_SHOW);
    SetWindowLongPtr(windows[id].hwnd, GWLP_USERDATA, (LONG_PTR)&windows[id]);

    if(event_callback) event_callback(id, WIN_EVENT_CREATE);

    return windows[id].id;
}

cstr_t* win_extensions(uint32_t* _extc) {
    *_extc = extension_count;
    return extensions;
}

void win_poll_events() {
    MSG msg = { 0 };

    if (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void win_del(uint32_t id) {
    if (!windows[id].hwnd) return;

    if (event_callback) {
        event_callback(id, WIN_EVENT_DELETE);
    }
   
    DestroyWindow(windows[id].hwnd);
    memset(&windows[id], 0, sizeof(win_data_t));
}

void win_get_extent(uint32_t id, vec2 out) {
    glm_vec2_copy(windows[id].extent, out);
}

void win_get_position(uint32_t id, vec2 out) {
    glm_vec2_copy(windows[id].position, out);
}

float win_get_aspect(uint32_t id) {
    return windows[id].aspect;
}

vptr_t win_get_handle(uint32_t id) {
    return windows[id].hwnd;
}

VkResult win_surface(uint32_t id, VkInstance instance, VkSurfaceKHR* surface) {
    if (!windows[id].hwnd) return VK_ERROR_INITIALIZATION_FAILED;

    VkWin32SurfaceCreateInfoKHR info = {
          .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
          .pNext = NULL,
          .hinstance = windows[id].hinstance,
          .hwnd = windows[id].hwnd
    };

    return vkCreateWin32SurfaceKHR(instance, &info, NULL, surface);
}

#endif
