#include "trigon/trigon.h"
#include "cstd/cstd.h"


static int main_local() {
    fs_t path = { 0 };
    fs_usr(path);
    printf("%s\n", path);


    return app_main();
}

#if defined(_NDEBUG) && defined(_WIN32) && defined(_CONSOLE_HIDDEN)
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    return main_local();
}
#else
int main() {
    return main_local();
}
#endif

