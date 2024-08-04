#ifdef _WIN32

#include <vulkan/vulkan_core.h>

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

#include "window.h"
#include "trigon/core/dev/gpu/gpu.h"


win_event_cb_t window_t::resize_callback = NULL;
void window_t::connect_resize(win_event_cb_t cb) { resize_callback = cb; }

cstr_t* window_t::extensions(uint32_t* count) {

#ifdef _DEBUG
    * count = 3;
    static cstr_t ext[3] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    return ext;

#else
    * count = 2;
    static cstr_t ext[3] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };
    return ext;
#endif
}

void window_t::sync(window_t* win) {
    if (!win) return;
    HWND hwnd = (HWND)win->handles[0];

    RECT    rect;
    POINT   pt = { 0, 0 };

    if (GetClientRect(hwnd, &rect)) {
        ClientToScreen(hwnd, &pt);

        u32 px = (u32)pt.x;
        u32 py = (u32)pt.y;
        u32 w = (u32)(rect.right - rect.left);
        u32 h = (u32)(rect.bottom - rect.top);

        if (w == win->width() && h == win->height()) return;

        win->properties[0] = px;
        win->properties[1] = py;
        win->properties[2] = w;
        win->properties[3] = h;

        if (resize_callback && (w != 0 || h != 0)) {
            resize_callback(win, WIN_EVENT::RESIZE_MOVE);
        }
    }
}

static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    window_t* ptr = (window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CLOSE:
        if (ptr) ptr->close();
        return 0;
    case WM_DWMWINDOWMAXIMIZEDCHANGE:
        if (ptr)window_t::sync(ptr);
        break;
    case WM_SIZE:
        if (ptr)window_t::sync(ptr);
        break;
    case WM_EXITSIZEMOVE:
        if (ptr)window_t::sync(ptr);
        break;
    case WM_DESTROY:
        if (ptr) ptr->close();
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void window_t::poll_events() {
    MSG msg = { 0 };

    if (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window_t::close() {
    if (resize_callback) {
        resize_callback(this, WIN_EVENT::DELETED);
    }

    if (surface) {
        vkDestroySurfaceKHR(vkinst_t::ref().vki, surface, NULL);
        surface = nullptr;
    }

    HWND hwnd = (HWND)handles[0];
    HINSTANCE hinstance = (HINSTANCE)handles[1];

    DestroyWindow(hwnd);
    memset(this, 0, sizeof(window_t));

    UnregisterClassA("WIN_CREATION", hinstance);
}

void window_t::create_surface() {
    HWND        hwnd = (HWND)handles[0];
    HINSTANCE   hinstance = (HINSTANCE)handles[1];

    VkWin32SurfaceCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = hinstance,
        .hwnd = hwnd
    };

    cassert(
        vkCreateWin32SurfaceKHR(
            vkinst_t::ref().vki,
            &info,
            NULL,
            &surface) == VK_SUCCESS,
        "failed to create window surface! [WIN32] \n"
    );
}

window_t::window_t() {
    HWND        hwnd = NULL;
    HINSTANCE   hinstance = NULL;

    hinstance = GetModuleHandle(NULL);
    WNDCLASSEXW wc = { 0 };

    wchar_t buffer[260] = { 0 };
    str_t::wchar(name, buffer);

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

    cassert(RegisterClassExW(&wc), "failed to create hinstance for WIN32");

    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        buffer,
        WS_OVERLAPPEDWINDOW,
        (int)properties[0],
        (int)properties[1],
        (int)properties[2],
        (int)properties[3],
        NULL, NULL, hinstance, NULL
    );

    cassert(hwnd, "failed to create hwnd for WIN32");

    handles[0] = hwnd;
    handles[1] = hinstance;

    ShowWindow((HWND)handles[0], SW_SHOW);
    SetWindowLongPtr((HWND)handles[0], GWLP_USERDATA, (LONG_PTR)this);

    sync(this);
    running = true;
    if (resize_callback) resize_callback(this, WIN_EVENT::CREATED);

    create_surface();
}


window_t::~window_t() {
    close();
    running = false;
}

void window_t::set_name(cstr_t new_name) {
    u64 len = strlen(new_name);
    cassert(
        len < WIN_MAX_NAME_LEN,
        "window_t::set_name buffer overflown! max %" PRId64 "\n",
        len
    );
    strcpy(name, new_name);

    wchar_t buffer[17] = {};
    str_t::wchar(name, buffer);
    SetWindowText((HWND)handles[0], buffer);
}


#endif