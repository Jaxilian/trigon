#include "cstd.h"
#include <stdio.h>
#include <string.h>
#include "errno.h"

static uint32_t log_index = 0;
static str256_t logs[DEBUG_MAX_LOGS] = { 0 };


void 
_debug_logger_add(DEBUG_TYPE type, cstr_t file, int line, cstr_t format, ...) {
    
    va_list args;
    va_start(args, format);

    char message[128];
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    char complete[128] = { 0 };
    switch (type){
        case DEBUG_LOG:
            sprintf(
                complete, "[LOG] %s >> %s:%d", message, file, line);
            break;
        case DEBUG_WRN:
            sprintf(complete, "[WRN] %s >> %s:%d", message, file, line);
            break;
        case DEBUG_ERR:
            sprintf(complete, "[ERR] %s >> %s:%d", message, file, line);
            break;
    default:
        break;
    }

    FILE* f = fopen("LOGS", "w");
    fclose(f);
    f = fopen("LOGS", "a");

    for (uint32_t i = DEBUG_MAX_LOGS-1; i > 0; i--) {
        strcpy(logs[i], logs[i - 1]);
        fputs(logs[i], f);
    }

    strcpy(logs[0], complete);
    fputs(logs[0], f);
    fclose(f);

#ifdef _DEBUG
    printf("%s >> system: %s\n", complete, strerror(errno));    
#endif
}