#include "trigon/trigon.h"

#include <stdio.h>

void app_update() {
    
}

void app_start() {
  
}

app_info_t app_main() {
    app_info_t app = {};
    app.name = "Editor";
    app.version = version_t(1, 0, 0, 0);
    app.update_cb = app_update;
    app.start_cb = app_start;
    return app;
}

