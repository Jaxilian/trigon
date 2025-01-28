#include "trigon/trigon.h"
#include "trigon/assets/assets.h"

void on_win_del(win_t* win) {
    shaders_clear();
}

int main(){
    gfx_init("Cards of power", (version_t){1,0,0,0});

    win_t win = { 0 };
    win_new(&win, "Window", 800, 600, 0);

    win.color[0] = 0.01f;
    win.color[1] = 0.01f;
    win.color[2] = 0.01f;
    win.color[3] = 1.00f;

    win_connect_to_del(&win, (win_del_cb)on_win_del);

    //compiles and builds assets for release (only runs in debug currently)
    tgn_assets_load("base", true);

    shader_info_t info  = { 
        .pack = "base",     // which pack owns this shader
        .name = "triangle", // name of shader file
        .window = &win,     // which window should it be rendered on
        .set_count = 1,
        .sets = {
            {
                .resource_count = 1,
                .resources = {
                    {
                        .count = 1,
                        .location = 0,
                        .buffer_type = SHADER_BUFFER_TYPE_STATIC
                    }
                }
            }
        },
    };

    shader_t shader = { 0 };
    shader_new(&shader, &info);

    // update loop
    while (win.active) {
        bool valid =  win_frame_begin(&win);
        if (!valid) continue;

        // test drawing
        tgn_upd_test(&shader);

        win_frame_end(&win);
    }
 
    win_del(&win);
    gfx_quit();

    return 0;
}