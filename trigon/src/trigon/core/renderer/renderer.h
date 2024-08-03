#pragma once

#include "vkdef.h"

#define WIN_MAX_NAME_LEN 16

class renderer_t;
class window_t;
class trigon_t;

/* ------------------------------ WINDOW ------------------------------------*/


enum WIN_EVENT {
    CREATED,
    DELETED,
    RESIZE_MOVE
};

typedef void (*win_event_cb_t)(window_t* win, WIN_EVENT ev);

class window_t {
public:

     window_t();
    ~window_t();

    static cstr_t* extensions(uint32_t* count);

    const u32& posx()   const { return properties[0]; }
    const u32& posy()   const { return properties[1]; }
    const u32& width()  const { return properties[2]; }
    const u32& height() const { return properties[3]; }
    const f32  aspect() const { return (f32)width() / (f32)height(); }

    void    close();
    vptr_t  handle(u32 id) { return handles[id]; }
    bool    active() { return running; }
    void    set_name(cstr_t new_name);

private:

    VkSurfaceKHR    surface         = NULL;
    vptr_t          handles[2]      = { NULL,NULL };
    bool            running         = false;

    char name[WIN_MAX_NAME_LEN] = "New Window";
    u32 properties[4] = {
        0,      // pos x
        0,      // pos y
        800,      // size x
        600       // size y
    };

    static win_event_cb_t resize_callback;
    static VkInstance      vkinstance;
    void create_surface(VkInstance instance);
   

public:
    static void connect_resize(win_event_cb_t cb);
    static void sync(window_t* win);
    static void poll_events();

    window_t(const window_t&) = delete;
    window_t& operator=(const window_t&) = delete;

    static window_t& main() {
        static window_t main_win;
        return main_win;
    }

    friend renderer_t;

};


/* ------------------------------ RENDERER ----------------------------------*/


class renderer_t {
public:
    VkInstance  vkinst = VK_NULL_HANDLE;



private:
     renderer_t() {};
    ~renderer_t();
    void init();

    bool initialized = false;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT vkdbg;
#endif
public:
    const cstr_t        ENGINE_NAME     = "Trigon";
    const version_t     ENGINE_VERSION  = version_t(1, 0, 0, 0);
    static cstr_t       APP_NAME;
    static version_t    APP_VERSION;

    renderer_t(const renderer_t&) = delete;
    renderer_t& operator=(const renderer_t&) = delete;
    renderer_t(renderer_t&&) = delete;
    renderer_t& operator=(renderer_t&&) = delete;

    static renderer_t& ref() {
        static renderer_t instance;
        return instance;
    }

    friend window_t;
    friend trigon_t;
};