#include "trigon/trigon.h"

int main(){
    gfx_init("Cards of power", (version_t){1,0,0,0});

    win_t win = { 0 };
    win_new(&win, "Window", 800, 600, 0);

    win.color[0] = 0.01f;
    win.color[1] = 0.01f;
    win.color[2] = 0.01f;
    win.color[3] = 1.0f;

    while (win.active) {
        win_frame_begin(&win);


        win_frame_end(&win);
    }
    
    win_del(&win);

    gfx_quit();
    return 0;
}