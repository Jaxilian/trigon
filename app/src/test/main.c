#include "trigon/trigon.h"

int local_main() {
    SERVICE_FLAGS services = 0;
    FLAG_ON(services, SERVICE_RENDERER);

    trigon_start(services);
    return 0;
}

#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hp, PWSTR cmd, int show) {
    return local_main();
}
#else
int main() {
    return local_main();
}
#endif
