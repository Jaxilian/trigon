<p align="center"><img align="center" src="https://github.com/Jaxilian/trigon/blob/main/trigon.png"/></p>

# Trigon

Trigon is a game engine, currently under development

## Description

A fun project

## Getting Started

Here's a small example on how to create a super small project with Trigon
```C
#include "trigon/trigon.h"
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

    shader_info_t info  = { 
        .pack = "base",     // which pack owns this shader
        .name = "triangle", // name of shader file
        .window = &win      // which window should it be rendered on

    };

    shader_t shader = { 0 };
    shader_new(&shader, &info);

    // update loop
    while (win.active) {
        win_frame_begin(&win);

        // test drawing
        tgn_upd_test(&shader);

        win_frame_end(&win);
    }
    
    shader_del(&shader);
    win_del(&win);
    gfx_quit();

    return 0;
}
```

### Dependencies

- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
- [Premake5](https://premake.github.io/)

### Building in a nutshell

#### Windows:
```
cd %USERPROFILE%/Downloads
git clone https://github.com/Jaxilian/trigon.git
cd trigon
premake5 vs2022
tgn.sln
```
#### Linux:
```
cd ~
git clone https://github.com/Jaxilian/trigon.git
cd trigon
premake5 gmake2
make
```

## Authors

Jax Carls

### Trigon uses the fellowing packages:
- [cglm](https://github.com/recp/cglm)
- [stb](https://github.com/nothings/stb)
- [cgltf](https://github.com/jkuhlmann/cgltf?tab=MIT-1-ov-file)
