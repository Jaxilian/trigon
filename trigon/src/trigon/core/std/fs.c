#include "std.h"

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

void fs_valid(fs_t in) {
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
    GetModuleFileNameA(NULL, dest, MAX_PATH);
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

void fs_append(fs_t dest, fs_t extra) {
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
    fs_valid(dest);
}

bool fs_find(fs_t path, fs_t child) {
    fs_t tmp = { 0 };
    fs_cd(tmp, path);
    fs_valid(tmp);
    fs_append(tmp, child);
    return fs_exist(tmp);
}

void fs_get(fs_t path, fs_t child) {
    fs_valid(path);
    fs_append(path, child);

    if (fs_exist(path)) {
        fs_valid(path);
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

    if (length >= __TG_FS_MAX) {
        length = __TG_FS_MAX - 1;
    }

    strncpy(dest, filename, length);
    dest[length] = '\0';
}

void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant) {
    memset(dest, 0, __TG_FS_MAX);
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

static int fs_lsrec(fs_t path, fs_t* out, const char* exc_ext, bool recursive, int* len) {
    DIR* d = opendir(path);
    if (!d) return 0;

    struct dirent* dir;
    fs_t e = { 0 };
    fs_t sub = { 0 };
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0) {
            continue;
        }

        memset(sub, 0, sizeof(fs_t));
        strcpy(sub, path);
        fs_valid(sub);
        strcat(sub, dir->d_name);

        if (!fs_isfile(sub) && recursive) {
            fs_lsrec(sub, out, exc_ext, recursive, len);
        }
        else {
            if (exc_ext != NULL) {
                fs_ext(e, dir->d_name);
                if (strcmp(e, exc_ext) != 0) {
                    continue;
                }
            }
            if (out) {
                strcpy(out[*len], sub);
            }
            (*len)++;
        }
    }

    closedir(d);
    return *len;
}

int  fs_ls(fs_t path, fs_t* out, const char* exc_ext, bool recursive) {
    int l = 0;
    fs_lsrec(path, out, exc_ext, recursive, &l);
    return l;
}
