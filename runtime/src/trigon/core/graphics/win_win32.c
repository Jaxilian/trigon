#ifdef _WIN32

#include "win.h"
#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "trigon/app.h"
#include "trigon/system/rendr.h"

#ifdef _DEBUG
static cstr_t	exts[] = { "VK_KHR_surface","VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
static uint32_t ext_c  = 3;
#else
static cstr_t	exts[] = { "VK_KHR_surface","VK_KHR_win32_surface" };
static uint32_t ext_c = 2;
#endif
static voidcb_t resized_cb = NULL;

void win_event_resized(voidcb_t cb) { resized_cb = cb; }

cstr_t* win_ext(uint32_t* _extc) {
    *_extc = ext_c;
    return exts;
}

void sync_resize(HWND hwnd) {
 
    win_t* ptr = (win_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!ptr) return;
    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        ptr->width = (uint32_t)width;
        ptr->height = (uint32_t)height;
        ptr->aspect = (float)ptr->width / (float)ptr->height;
        if (resized_cb && !app_quitting()) resized_cb();
    }
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_CLOSE:
        app_quit();
        return 0;
    case WM_SIZE:
        sync_resize(hwnd);
        break;
    case WM_SIZING:
        sync_resize(hwnd);
        break;
    case WM_WINDOWPOSCHANGED:
        sync_resize(hwnd);
        break;
    case WM_DWMWINDOWMAXIMIZEDCHANGE:
        sync_resize(hwnd);
        break;
    case WM_EXITSIZEMOVE:
        sync_resize(hwnd);
        break;
    case WM_DESTROY:
        app_quit();
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void win_new(win_t* ptr) {
    HINSTANCE hinstance = GetModuleHandle(NULL);
    WNDCLASSEXW wc = {0};
    HWND hwnd;

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
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        L"window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hinstance, NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ptr);

    ptr->window_handle  = hwnd;
    ptr->width          = 800;
    ptr->height         = 600;
    ptr->aspect         = (float)ptr->width /(float)ptr->height;

    VkWin32SurfaceCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = hinstance,
        .hwnd = hwnd
    };

    vkCreateWin32SurfaceKHR(rendr_get()->device.instance, &info, NULL, &ptr->surface);
}

void win_upd(win_t* ptr) {
    MSG msg = {0};

    if (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void win_del(win_t* ptr) {
    if (!ptr || !ptr->window_handle) return;
    vkDestroySurfaceKHR(rendr_get()->device.instance, ptr->surface, NULL);
    ptr->surface = NULL;

    DestroyWindow((HWND)ptr->window_handle);
    ptr->window_handle = NULL;
}

#endif