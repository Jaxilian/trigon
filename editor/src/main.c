

int app_main() {
  
    return 0;
}


#ifdef _WIN32
#ifdef _DEBUG
int main() {
    return app_main();
}
#else
#include <windows.h>
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return app_main();
}
#endif
#else
int main() {
    return app_main();
}
#endif