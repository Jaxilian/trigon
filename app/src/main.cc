#include "trigon/trigon.h"

#include <stdio.h>

app_info_t app_main() {
    app_info_t app = {};
    app.name = "Editor";
    app.version = version_t(1, 0, 0, 0);



    return app;
}

