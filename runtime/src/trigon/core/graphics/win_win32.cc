
#ifdef _WIN32 
#include "win.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "gpu.h"


#ifdef _DEBUG
const char*     win_t::extensions[] = { "VK_KHR_surface","VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
int             win_t::extension_count = 3;
#else
const char*     win_t::extensions[] = {"VK_KHR_surface","VK_KHR_win32_surface"};
int             win_t::extension_count = 2;
#endif

static std::vector<win_t*> _win_instances;

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CLOSE:
        for (int i = 0; i < _win_instances.size(); i++) {
            if (_win_instances[i]->native_ptr() == hwnd) {
                _win_instances[i]->close();
                break;
            }
        }
        return 0;
    case WM_ENTERSIZEMOVE:

        break;
    case WM_DESTROY:
        for (int i = 0; i < _win_instances.size(); i++) {
            if (_win_instances[i]->native_ptr() == hwnd) {
                _win_instances[i]->close();
                break;
            }
        }
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void win_t::resize(int width, int height) {
    _width = width;
    _height = height;

    SetWindowPos(
        (HWND)_handle,
        HWND_TOP,
        0, 0,
        _width, _height,
        SWP_NOMOVE | SWP_NOZORDER
    );
}

bool win_t::_init() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEXW wc = {};
    HWND hwnd;

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    std::wstring wclassname = L"dwl::win_t";
    wc.lpszClassName = wclassname.c_str();
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Register the window class
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        L"new window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, _width, _height,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);


    _win_instances.push_back(this);
    _handle = hwnd;
    update();

    VkWin32SurfaceCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.hinstance = hInstance;
    info.hwnd = hwnd;

    VkSurfaceKHR surf;
    vkCreateWin32SurfaceKHR((VkInstance)gpu_t::get().instance, &info, NULL, &surf);
    _surface = surf;
    
    gpu_t::get().bind_device(_surface);

    return true;
}

void win_t::update() {
    MSG msg = {};

    if (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void win_t::close() {
    _closing = true;
}

win_t::win_t() :_gpu(gpu_t::get()) {
    _init();
}

win_t::~win_t() {
    vkDestroySurfaceKHR((VkInstance)gpu_t::get().instance, (VkSurfaceKHR)_surface, NULL);
    for (int i = 0; i < _win_instances.size(); i++) {
        if (_win_instances[i] == this) {
            _win_instances.erase(_win_instances.begin() + i);
            break;
        }
    }

    DestroyWindow((HWND)_handle);
    _handle = nullptr;
}

#endif
