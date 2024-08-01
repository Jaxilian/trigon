#include "trigon/trigon.h"
#include <stdio.h>

static cstr_t			ENGINE_NAME = "Trigon";
static const version_t	ENGINE_VERSION = version_t(1,0,0,0);

int trigon_main() {
    app_t app = app_main();
    printf("%s : %s\n", ENGINE_NAME, ENGINE_VERSION.str());
    printf("%s : %s\n", app.name, app.version.str());

    return 0;
}


#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hp, PWSTR cmd, int show) {
    return trigon_main();
}
#else
int main() {
    return trigon_main();
}
#endif