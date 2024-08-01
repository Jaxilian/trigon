#include "trigon/trigon.h"

int trigon_main() {
    return app_main();
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