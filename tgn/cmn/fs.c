#include "fs.h"

#ifdef _WIN32
#define USE_DIRENT_WIN32
#include "dirent.h"
#include <direct.h>
#include <shlobj_core.h>
#else
#include <unistd.h> 
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32 
#define __FS_STD_DIREND '\\'
#define __FS_NSTD_DIREND '/'
#else
#define __FS_STD_DIREND '/'
#define __FS_NSTD_DIREND '\\'
#endif

typedef struct {
    int     len;
    int     max;
    fs_t* entries;
} fs_ls_entries_left_t;

void fs_validate(fs_t in) {
    size_t str_len = strlen(in);
    for (int i = 0; i < str_len; i++)
        if (in[i] == __FS_NSTD_DIREND) in[i] = __FS_STD_DIREND;

    if (!fs_exist(in)) return;

    if (fs_isfile(in)) {
        if (in[str_len - 1] != __FS_STD_DIREND) return;
        in[str_len - 1] = '\0';
    }
    else {
        if (in[str_len - 1] == __FS_STD_DIREND) return;
        in[str_len] = __FS_STD_DIREND;
        in[str_len + 1] = '\0';
    }

}

void fs_parent(fs_t dest, fs_t path) {
    size_t len = strlen(path);
    strcpy(dest, path);
    for (size_t i = len; i > 0; i--) {
        if (dest[i] == __FS_STD_DIREND) {
            dest[i + 1] = '\0';
            return;
        }
    }
}

void fs_app(fs_t dest) {
#ifdef _WIN32
    GetModuleFileNameA(NULL, dest, __STD_FS_MAX);
#else
    readlink("/proc/self/exe", p, __TG_FS_MAX);
#endif
}

void fs_dir(fs_t dest) {
#ifdef _WIN32
    char* buffer;
    if ((buffer = _getcwd(NULL, 0)) == NULL) {
        memset(dest, 0, sizeof(fs_t));
        return;
    }

    strcpy(dest, buffer);
    free(buffer);

#else
    getcwd(dest, __TG_FS_MAX);
#endif
}

bool fs_isfile(fs_t dest) {
    struct stat path_stat = { 0 };
    stat(dest, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void fs_usr(fs_t dest) {
#ifdef _WIN32
    SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, dest);
#else
    const char* home = getenv("HOME");
    if (home) strcpy(dest, home);
    else {
        const char* user_profile = getenv("USERPROFILE");
        if (user_profile) {
            strcpy(dest, user_profile);
        }
        else {
            dest[0] = '\0';
        }
    }
#endif
}

void fs_root(fs_t dest) {
#ifdef _WIN32
    strcpy(dest, "/");
#else
    strcpy(dest, "C:\\");
#endif
}

void fs_add(fs_t dest, fs_t extra) {
    strcat(dest, extra);
}

bool fs_exist(fs_t dest) {
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(dest);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    if (fs_isfile(dest)) return access(dest, F_OK) == 0;
    else return opendir(dest) != NULL;
#endif
}


void fs_cd(fs_t dest, fs_t src) {
    strcpy(dest, src);
    fs_validate(dest);
}

bool fs_find(fs_t path, fs_t child) {
    fs_t tmp = { 0 };
    fs_cd(tmp, path);
    fs_validate(tmp);
    fs_add(tmp, child);
    return fs_exist(tmp);
}

void fs_get(fs_t path, fs_t child) {
    fs_validate(path);
    fs_add(path, child);

    if (fs_exist(path)) {
        fs_validate(path);
        return;
    }

    if (path[strlen(path) - 1] == __FS_STD_DIREND) {
        int result = _mkdir(path);
        return;
    }

    FILE* f = fopen(path, "w");
    if (!f) return;
    fclose(f);
}


void fs_ext(fs_t dest, fs_t path) {
    const char* dot = strrchr(path, '.');
    if (!dot || dot == path) {
        memset(dest, 0, sizeof(fs_t));
        return;
    }

    strcpy(dest, dot + 1);
}

void fs_name(fs_t dest, fs_t path) {
    const char* filename = strrchr(path, '/');
    if (filename) {
        filename++;
    }
    else {
        filename = path;
    }

    const char* ext = strrchr(filename, '.');
    size_t length;
    if (ext) {
        length = ext - filename;
    }
    else {
        length = strlen(filename);
    }

    if (length >= __STD_FS_MAX) {
        length = __STD_FS_MAX - 1;
    }

    strncpy(dest, filename, length);
    dest[length] = '\0';
}

void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant) {
    memset(dest, 0, __STD_FS_MAX);
    int tl = 0;
    int al = (int)strlen(ancestor);
    int ad = (int)strlen(descandant);
    int mx = MAX(al, ad);
    for (int i = 0; i < mx; i++) {
        if (i < ad && i < al && ancestor[i] == descandant[i]) continue;
        dest[tl] = descandant[i];
        tl++;
    }
}

int fs_ls(fs_t inpath, fs_t* out, const char* exc_ext, bool recursive) {
    fs_t path = { 0 };
    strcpy(path, inpath);
    fs_validate(path);

    struct dirent* dir;
    fs_t extension = { 0 };
    fs_t subdir = { 0 };
    int len = 0;
    int too_long = 0;

    fs_ls_entries_left_t left = { 0 };
    left.max = 64;
    left.len = 0;
    left.entries = malloc(sizeof(fs_t) * left.max);

    if (!left.entries) return 0;

    strcpy(left.entries[left.len++], path);

    while (left.len > 0) {
        strcpy(path, left.entries[--left.len]);
        DIR* d = opendir(path);

        if (!d) {
            continue;
        }

        int path_len = (int)strlen(path);

        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0) {
                continue;
            }

            int name_len = (int)strlen(dir->d_name);
            if (path_len + name_len >= __STD_FS_MAX - 1) {
                too_long++;
                continue;
            }

            snprintf(subdir, sizeof(fs_t), "%s%s", path, dir->d_name);
            fs_validate(subdir);
            fs_ext(extension, dir->d_name);

            bool isfile = fs_isfile(subdir);

            if (!isfile && recursive && strlen(extension) == 0) {
                if (left.len >= left.max) {
                    fs_t* tmp = realloc(left.entries, sizeof(fs_t) * left.max * 2);
                    if (!tmp) {
                        free(left.entries);
                        closedir(d);
                        printf("realloc failed! returning 0 from fs_ls()\n");
                        return 0;
                    }
                    left.entries = tmp;
                    left.max = left.max * 3 / 2;
                }

                strcpy(left.entries[left.len++], subdir);
            }
            else {
                if (exc_ext && strcmp(extension, exc_ext) != 0) {
                    continue;
                }

                if (out) {
                    strcpy(out[len], subdir);
                }
                len++;
            }
        }
        closedir(d);
    }

    if (too_long > 0) {
        printf("%d paths where longer than %d! Those paths were skipped...\n", too_long, __STD_FS_MAX);
    }

    free(left.entries);
    return len;
}

void fs_pop(fs_t path) {
    size_t len = strlen(path);

    while (len > 0 && path[len - 1] == __FS_STD_DIREND) {
        path[--len] = '\0';
    }

    char* last_separator = strrchr(path, __FS_STD_DIREND);
    if (last_separator != NULL) {
        *last_separator = '\0';
    }
}