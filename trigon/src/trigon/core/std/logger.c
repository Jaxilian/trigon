#include "std.h"
#include <stdio.h>
#include <string.h>
#include "errno.h"
#include <stdlib.h>


#define MAX_LOG_SIZE 1024

typedef char log_msg_t[MAX_LOG_SIZE];

static u32     log_len = 0;
static u32     log_max = 0;
static u32     log_nxt = 32;
static log_msg_t* logs = NULL;

void debug_log_clear() {
    FILE* f = fopen("LOGS", "w");
    fclose(f);

    if (logs) {
        free(logs);
        logs = NULL;
    }
}

void
_debug_logger_add(bool force_quit,
    DEBUG_TYPE type, cstr_t file, int line, cstr_t format, ...) {

    va_list args;
    va_start(args, format);

    char message[MAX_LOG_SIZE];
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    char complete[MAX_LOG_SIZE] = { 0 };
    switch (type) {
    case DEBUG_LOG:
        sprintf(
            complete, "[LOG] %s", message);
        break;
    case DEBUG_WRN:
        sprintf(complete, "[WRN] %s >> %s:%d\n\n", message, file, line);
        break;
    case DEBUG_ERR:
        sprintf(complete, "[ERR] %s >> %s:%d\n\n", message, file, line);
        break;
    default:
        break;
    }

    FILE* f = fopen("LOGS", "w");
    fclose(f);
    f = fopen("LOGS", "a");


    u32 new_len = log_len + 1;
    if (new_len >= log_max) {
        size_t size_addon = sizeof(log_msg_t) * log_nxt;
        size_t size_current = sizeof(log_msg_t) * log_max;
        size_t size_new = size_current + size_addon;

        if (logs) {
            log_msg_t* new_logs = (log_msg_t*)realloc(logs, size_new);

            if (!new_logs) {
                printf("failed to alloc memory for logger!\n");
                exit(1);
            }

            logs = new_logs;
        }
        else {
            logs = (log_msg_t*)malloc(size_new);
        }
    };

    if (!logs) {
        printf("failed to get memory for logs, should not happen!\n");
        exit(1);
    }

    for (u32 i = log_len; i > 0; i--) {
        strcpy(logs[i], logs[i - 1]);
        fputs(logs[i], f);
    }

    log_len = new_len;
    strcpy(logs[0], complete);
    printf("%s", complete);
    fputs(logs[0], f);
    fclose(f);

    if (force_quit) exit(-1);
}