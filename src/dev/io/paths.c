#include "io.h"
#include <string.h>
#include <stdlib.h>

#pragma warning (disable:4090)
#pragma warning (disable:4133)
#pragma warning (disable:4477)

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>

static wchar_t* string_convert_to_wide_char(const char* str) {
    // First, find the length needed for the wide character string
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (len == 0) {
        return NULL;
    }

    // Allocate memory for the wide character string
    wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (wstr == NULL) {
        return NULL;
    }

    // Convert the ANSI string to wide character string
    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len) == 0) {
        free(wstr);
        return NULL;
    }

    return wstr;
}

static void win_get_subdirs(const char* _path) {
    wchar_t* path = string_convert_to_wide_char(_path);
    if (path == NULL) {
        printf("String conversion failed.\n");
        return;
    }

    WIN32_FIND_DATAW find_file_data;
    HANDLE h_find = INVALID_HANDLE_VALUE;

    wchar_t search_path[MAX_PATH];
    swprintf(search_path, MAX_PATH, L"%s\\*", path);

    h_find = FindFirstFileW(search_path, &find_file_data); 

    if (h_find == INVALID_HANDLE_VALUE) {
        wprintf(L"FindFirstFile failed (%d)\n", GetLastError());
        free(path);
        return;
    }

    do {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
  
            if (wcscmp(find_file_data.cFileName, L".") != 0 && wcscmp(find_file_data.cFileName, L"..") != 0) {
                wprintf(L"%ls\n", find_file_data.cFileName); 
            }
        }
        else {
            wprintf(L"%ls\n", find_file_data.cFileName);
        }
    } while (FindNextFileW(h_find, &find_file_data));

    FindClose(h_find);
    free(path);
}
#endif

static char root_path[64];

static void split_path(cstr str, cstr delimiter, cstr output[32], uint32_t* count) {
    *count = 0;  
    cstr last_non_delim = str;

    while (*str != '\0') {
        if (strchr(delimiter, *str) != NULL) {
            if (last_non_delim != str) {
                output[(*count)++] = last_non_delim;
                if (str != last_non_delim) {
                    *((char*)str) = '\0';
                }
            }
            last_non_delim = str + 1;
        }
        str++;
    }
    if (last_non_delim != str) {
        output[(*count)++] = last_non_delim;
    }
}



void paths_new(cstr path) {

    cstr parts[32];
    uint32_t count = 0; 
    split_path(path, "\\", parts, &count);

    uint32_t index = 0;
	for (uint32_t i = 0; i < count-1; i++) {
        size_t len = strlen(parts[i]);
        for (size_t j = 0; j < len; j++) {
            root_path[index] = parts[i][j];
            index++;
        }
        root_path[index] = '\\';
        index++;
	}
	debug_log("%s\n", root_path);
    
    win_get_subdirs(root_path);
}

void paths_del() {

}
#pragma warning (default:4090)
#pragma warning (default:4133)
#pragma warning (default:4477)