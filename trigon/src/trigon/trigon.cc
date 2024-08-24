#include "trigon/trigon.h"
#include <stdio.h>

#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif


#include "trigon/script/script.h"

int trigon_main() {

    app_info_t info = app_init();
    printf("hello!\n");

    script_t script;
    
    
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

