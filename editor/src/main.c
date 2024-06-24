#include "trigon/app.h"

void engine_start() {

}

void engine_update() {

}

void engine_quit() {

}

int main(){
    app_info_t info = {
        .version    = {1,0,0,0},
        .name       = "engine",
        .on_start   = engine_start,
        .on_update  = engine_update,
        .on_quit    = engine_quit
    };

    app_load(&info);
    return 0;
}