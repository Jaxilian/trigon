#include "cstd.h"
#include <stdio.h>
#include <string.h>
#include "errno.h"
#include <stdlib.h>

static uint32_t  log_len     = 0;
static uint32_t  log_max     = 0;
static uint32_t  log_nxt     = 32;
static str256_t* logs        = NULL;

void debug_log_clear() {
    FILE* f = fopen("LOGS", "w");
    fclose(f);

    if (logs) {
        free(logs);
        logs = NULL;
    }
}

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
                complete, "[LOG] %s", message);
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


    uint32_t new_len = log_len + 1;
    if (new_len >= log_max) {
        size_t size_addon   = sizeof(str256_t) * log_nxt;
        size_t size_current = sizeof(str256_t) * log_max;
        size_t size_new = size_current + size_addon;

        if (logs) {
            str256_t* new = realloc(logs, size_new);

            if (!new) {
                printf("failed to alloc memory for logger!\n");
                exit(1);
            }

            logs = new;
        }
        else {
            logs = malloc(size_new);
        }
    };
    if (!logs) {
        printf("failed to get memory for logs, should not happen!\n");
        exit(1);
    }

    for (uint32_t i = log_len; i > 0; i--) {
        strcpy(logs[i], logs[i - 1]);
        fputs(logs[i], f);
    }
    log_len = new_len;
    strcpy(logs[0], complete);
    printf("%s", complete);
    fputs(logs[0], f);
    fclose(f);

#ifdef _DEBUG
    if (type == DEBUG_ERR) {
        printf("%s >> system: %s\n", complete, strerror(errno));    
    }
#endif
}