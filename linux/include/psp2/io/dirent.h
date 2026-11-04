#ifndef __PSP2_IO_DIRENT_H__
#define __PSP2_IO_DIRENT_H__

#include <dirent.h>
#include <linux_vita_shim.h>

typedef struct SceIoDirent {
    SceIoStat d_stat;
    char d_name[256];
} SceIoDirent;

#define MAX_VITA_DIRS 32
static DIR *s_vita_dirs[MAX_VITA_DIRS];

static inline SceUID sceIoDopen(const char *dirname) {
    DIR *d = opendir(vita_to_linux_path(dirname));
    if (!d) return -1;
    for (int i = 0; i < MAX_VITA_DIRS; ++i) {
        if (!s_vita_dirs[i]) {
            s_vita_dirs[i] = d;
            return (SceUID)(i + 1);
        }
    }
    closedir(d);
    return -1;
}

static inline int sceIoDclose(SceUID d) {
    int idx = (int)d - 1;
    if (idx >= 0 && idx < MAX_VITA_DIRS && s_vita_dirs[idx]) {
        closedir(s_vita_dirs[idx]);
        s_vita_dirs[idx] = NULL;
    }
    return 0;
}

static inline int sceIoDread(SceUID d, SceIoDirent *dir) {
    int idx = (int)d - 1;
    if (idx < 0 || idx >= MAX_VITA_DIRS || !s_vita_dirs[idx] || !dir) return 0;
    struct dirent *de = readdir(s_vita_dirs[idx]);
    if (!de) return 0;
    memset(&dir->d_stat, 0, sizeof(dir->d_stat));
    strncpy(dir->d_name, de->d_name, sizeof(dir->d_name) - 1);
    dir->d_name[sizeof(dir->d_name) - 1] = 0;

    int dir_fd = dirfd(s_vita_dirs[idx]);
    struct stat s;
    if (dir_fd >= 0 && fstatat(dir_fd, de->d_name, &s, 0) == 0) {
        dir->d_stat.st_mode = s.st_mode;
        dir->d_stat.st_size = s.st_size;
        time_t sec = s.st_mtim.tv_sec;
        struct tm tm_val;
        localtime_r(&sec, &tm_val);
        dir->d_stat.st_mtime.year = tm_val.tm_year + 1900;
        dir->d_stat.st_mtime.month = tm_val.tm_mon + 1;
        dir->d_stat.st_mtime.day = tm_val.tm_mday;
        dir->d_stat.st_mtime.hour = tm_val.tm_hour;
        dir->d_stat.st_mtime.minute = tm_val.tm_min;
        dir->d_stat.st_mtime.second = tm_val.tm_sec;
    } else {
        if (de->d_type == DT_DIR) {
            dir->d_stat.st_mode = S_IFDIR;
        } else {
            dir->d_stat.st_mode = S_IFREG;
        }
    }
    return 1;
}

#endif // __PSP2_IO_DIRENT_H__
