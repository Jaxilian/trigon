#include "trigon/trigon.h"
#include "trigon/shading/shaders.h"
#include "trigon/assets/assets.h"

int main(){
    gfx_init("Cards of power", (version_t){1,0,0,0});

    win_t win = { 0 };
    win_new(&win, "Window", 800, 600, 0);

    win.color[0] = 0.01f;
    win.color[1] = 0.01f;
    win.color[2] = 0.01f;
    win.color[3] = 1.00f;

    //compiles and builds assets for release
    tgn_assets_load("base", true);

    // creates a new shader from compilation
    shader_info_t info  = { 
        .pack = "base", // where the shader lies
        .name = "triangle", // name of shader file
        .window = &win // window that contains render pass 

    };

    shader_t shader = { 0 };
    shader_new(&shader, &info);

    // update loop
    while (win.active) {
        win_frame_begin(&win);

        win_frame_end(&win);
    }
    
    shader_del(&shader);
    win_del(&win);
    gfx_quit();

    return 0;
}