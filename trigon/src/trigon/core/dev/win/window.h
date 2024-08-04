#pragma once
#include "trigon/core/std/std.h"
#include "trigon/core/dev/gpu/vk.h"

#define WIN_MAX_NAME_LEN 16
class renderer_t;
class window_t;
class swap_t;

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

    u32 posx()   const { return properties[0]; }
    u32 posy()   const { return properties[1]; }
    u32 width()  const { return properties[2]; }
    u32 height() const { return properties[3]; }
    f32 aspect() const { return (f32)width() / (f32)height(); }

    void    close();
    vptr_t  handle(u32 id) { return handles[id]; }
    bool    active() { return running; }
    void    set_name(cstr_t new_name);

    VkSurfaceKHR    surface = NULL;

private:
    vptr_t          handles[2] = { NULL,NULL };
    bool            running = false;

    char name[WIN_MAX_NAME_LEN] = "New Window";
    u32 properties[4] = {
        0,      // pos x
        0,      // pos y
        800,      // size x
        600       // size y
    };

    static win_event_cb_t resize_callback;
    void create_surface();
    void destroy_surface();

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
