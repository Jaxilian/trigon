#include <stdio.h>


static int main_local() {
    printf("%s\n", "hello world");
    return 0;
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
