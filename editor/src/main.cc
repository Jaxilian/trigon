#include "trigon/app.h"
#include "trigon/shaders/shader.h"

int app_main() {

    shader_t shader;
    shader.add_resource(0, SHADER_PROP::dynamic_buffer);
    shader.bind_resources();
    

    return 0;
}




#ifdef _DEBUG
    int main() {
        return app_main();
    }
#else
    #ifdef _WIN32
        #include <windows.h>
        int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
        {
            return app_main();
        }
    #endif
#endif

