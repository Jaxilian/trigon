#pragma once
#include "trigon/core/memory/pointer.h"

class gpu_t;
class win_t {
public:

     win_t();
    ~win_t();

    void  resize(int width, int height);
    void  update();
    void  close();
    bool  closing()     { return _closing; }
    void* native_ptr()  { return _handle; }
    void* surface_ptr() { return _surface; }

    static const char*  extensions[];
    static int          extension_count;

private:
    bool    _init();

    int         _posw = 0;
    int         _posh = 0;
    int         _width = 800;
    int         _height = 600;
    float       _aspect = (800.0f / 600.0f);
    void*       _handle = nullptr;
    void*       _surface = nullptr;
    bool        _closing = false;
    gpu_t&      _gpu;

    ptr_t<void*> _swapchain;
};
