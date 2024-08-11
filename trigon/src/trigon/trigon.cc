#include "trigon/trigon.h"
#include <stdio.h>

#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include <SDL3/SDL.h>
#include "trigon/script/script.h"

int trigon_main() {
    app_info_t info = app_init();
    printf("%d\n", info.test);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "hello",
        800,
        600,
        SDL_WINDOW_RESIZABLE
    );

    script_t script;

    SDL_Event ev;
    bool running = true;

    while (running) {
        SDL_PollEvent(&ev);
        while (SDL_PollEvent(&ev)) {
            switch (ev.type){
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
            }
        }

    }

    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

#if defined(_WIN32) && defined(_NDEBUG)
int APIENTRY WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return trigon_main();
}

#else
int main() {
    return trigon_main();
}

#endif

