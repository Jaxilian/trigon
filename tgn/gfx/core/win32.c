#ifdef _WIN32

#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "../gfx.h"
#include "cmn/cmn.h"

static str_t CLASS_NAME = "WIN32_WIN_CLASS";
static MSG msg          = { 0 };

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    win_t* win = (win_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (uMsg){

        case WM_SIZE: {
            uint32_t width = LOWORD(lParam);
            uint32_t height = HIWORD(lParam);

            win->height = height;
            win->width  = width;
            win->aspect = (float)win->width / (float)win->height;

            if (win->callback) {
                win->callback(win);
            }

            gfx_swap_sync(win);

            return 0;
        }

        case WM_CLOSE:
            win->active = false;
            win_del(win);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0); 
            return 0;

    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void win_new(win_t* win, str_t win_name, uint32_t width, uint32_t height, uint32_t flags) {

    WNDCLASSW wc = {
       .lpfnWndProc = WindowProc,
       .hInstance = win->instance,
       .lpszClassName = L"WIN32_WIN_CLASS"
    };

    RegisterClassW(&wc);

    wchar_t wide_name[256];
    MultiByteToWideChar(CP_UTF8, 0, win_name, -1, wide_name, 256);

    win->handle = (HWND)CreateWindowExW(
        0,
        L"WIN32_WIN_CLASS",
        wide_name,                     
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, win->instance, win
    );

    DWORD error = GetLastError();
    assert(win->handle != NULL, "failed to create window, error: %lu\n", error);

    SetWindowLongPtr(win->handle, GWLP_USERDATA, (LONG_PTR)win);
    ShowWindow(win->handle, true);
    win->active = true;

    win->height = height;
    win->width  = width;
    win->aspect = (float)win->width / (float)win->height;

    VkWin32SurfaceCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = win->instance,
        .hwnd = win->handle
    };

    if (gfx_vkinstance() == NULL) {
        debug_err("Call gfx_init() before calling win_new()!\n");
    }

    vkCreateWin32SurfaceKHR(gfx_vkinstance(), &info, NULL, &win->surface);
    win->device = gfx_device_get(win);
    gfx_swap_sync(win);
    gfx_sync_objects_create(win);
    gfx_load_requirements();
}

void win_frame_begin(win_t* win) {

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
            win->active = false; 
            win_del(win);
            break;
        }
    }

    gfx_frame_begin(win);
}

void win_frame_end(win_t* win) {
    gfx_frame_end(win);
}

void win_del(win_t* win) {
    gfx_unload_requirements();
    gfx_swap_del(win);
    gfx_sync_objects_destroy(win);
    gfx_device_del(win);

    if (win->surface) {
        vkDestroySurfaceKHR(gfx_vkinstance(), win->surface, NULL);
        win->surface = NULL;
    }

    if (win->handle) {
        DestroyWindow(win->handle);
        win->handle = NULL;
    }

    UnregisterClassA(CLASS_NAME, win->instance);
}

#endif