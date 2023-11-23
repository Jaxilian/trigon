#include "std.h"
#include <string.h>
#include <stdlib.h>

void split_string(cstr* str, cstr* delimiter, char** buffer, uint32_t buffer_len) {

    char* str_copy = strdup((const char*)str);
    validate(str_copy != NULL, "Memory allocation failed\n");

    char* token = strtok(str_copy, (const char*)delimiter);
    uint32_t buffer_index = 0;
    while (token != NULL) {
        validate(buffer_index < buffer_len, "buffer overflown!\n");
        strcpy(buffer[buffer_index], token);
        buffer_index++;
        token = strtok(NULL, (const char*)delimiter);
    }

    free(str_copy);
}


