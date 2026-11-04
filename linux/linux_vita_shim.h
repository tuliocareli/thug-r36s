#ifndef __LINUX_VITA_SHIM_H__
#define __LINUX_VITA_SHIM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <SDL.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

// Definições de tipos Vita / SCE
typedef int32_t SceUID;
typedef uint32_t SceUInt32;
typedef uint64_t SceUInt64;
typedef int32_t SceInt32;
typedef int64_t SceInt64;
typedef size_t SceSize;
typedef int SceAudioOutChannelFlag;

#define SCE_OK 0
#define SCE_O_RDONLY O_RDONLY
#define SCE_O_WRONLY O_WRONLY
#define SCE_O_RDWR   O_RDWR
#define SCE_O_APPEND O_APPEND
#define SCE_O_CREAT  O_CREAT
#define SCE_O_TRUNC  O_TRUNC

typedef struct SceDateTime {
    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
    unsigned int microsecond;
} SceDateTime;

#ifdef st_mtime
#undef st_mtime
#endif
#ifdef st_atime
#undef st_atime
#endif
#ifdef st_ctime
#undef st_ctime
#endif

typedef struct SceIoStat {
    mode_t st_mode;
    off_t st_size;
    SceDateTime st_mtime;
} SceIoStat;

#include <dirent.h>

#define PATH_CACHE_SIZE 512
typedef struct {
    char in[256];
    char out[256];
} PathCacheEntry;

static PathCacheEntry s_path_cache[PATH_CACHE_SIZE];
static int s_path_cache_count = 0;
static pthread_mutex_t s_path_cache_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline const char *vita_to_linux_path(const char *path) {
    if (!path || !path[0]) return "";

    const char *p = path;
    if (strncmp(p, "ux0:data/thug/", 14) == 0) p += 14;
    else if (strncmp(p, "ux0:data/", 9) == 0) p += 9;
    else if (strncmp(p, "ux0:", 4) == 0) p += 4;

    struct stat st;
    if (::stat(p, &st) == 0) return p;

    char clean[256];
    size_t len = 0;
    while (p[len] && len < sizeof(clean) - 1) {
        clean[len] = (p[len] == '\\') ? '/' : p[len];
        len++;
    }
    clean[len] = '\0';

    if (::stat(clean, &st) == 0) {
        return p;
    }

    pthread_mutex_lock(&s_path_cache_mutex);
    for (int i = 0; i < s_path_cache_count; ++i) {
        if (strcmp(s_path_cache[i].in, clean) == 0) {
            const char *res = s_path_cache[i].out;
            pthread_mutex_unlock(&s_path_cache_mutex);
            return res;
        }
    }
    pthread_mutex_unlock(&s_path_cache_mutex);

    char current[256];
    current[0] = '\0';

    const char *cursor = clean;
    while (*cursor) {
        while (*cursor == '/') cursor++;
        if (!*cursor) break;

        const char *slash = strchr(cursor, '/');
        size_t part_len = slash ? (size_t)(slash - cursor) : strlen(cursor);
        char part[128];
        if (part_len >= sizeof(part)) part_len = sizeof(part) - 1;
        strncpy(part, cursor, part_len);
        part[part_len] = '\0';
        bool is_last = (slash == NULL || *(slash + 1) == '\0');

        char dir_to_open[256];
        if (current[0] == '\0') strcpy(dir_to_open, ".");
        else snprintf(dir_to_open, sizeof(dir_to_open), "%s", current);

        DIR *d = opendir(dir_to_open);
        bool found = false;
        char matched[128];

        if (d) {
            struct dirent *de;
            while ((de = readdir(d)) != NULL) {
                if (strcasecmp(de->d_name, part) == 0) {
                    strncpy(matched, de->d_name, sizeof(matched) - 1);
                    matched[sizeof(matched) - 1] = '\0';
                    found = true;
                    break;
                }
                if (is_last) {
                    size_t de_l = strlen(de->d_name);
                    size_t pt_l = strlen(part);
                    if (pt_l > 4 && strcasecmp(part + pt_l - 4, ".xbx") == 0) {
                        if (strncasecmp(de->d_name, part, pt_l - 4) == 0 && de_l == pt_l - 4) {
                            strncpy(matched, de->d_name, sizeof(matched) - 1);
                            matched[sizeof(matched) - 1] = '\0';
                            found = true;
                            break;
                        }
                    }
                    if (pt_l >= 8 && de_l >= 8 &&
                        strcasecmp(part + pt_l - 4, ".bik") == 0 &&
                        strcasecmp(de->d_name + de_l - 4, ".bik") == 0) {
                        if (strncasecmp(de->d_name, part, 8) == 0) {
                            strncpy(matched, de->d_name, sizeof(matched) - 1);
                            matched[sizeof(matched) - 1] = '\0';
                            found = true;
                            break;
                        }
                    }
                }
            }
            closedir(d);
        }

        if (current[0] != '\0') {
            size_t c_l = strlen(current);
            if (c_l < sizeof(current) - 2) {
                current[c_l] = '/';
                current[c_l + 1] = '\0';
            }
        }
        strncat(current, found ? matched : part, sizeof(current) - strlen(current) - 1);

        if (!slash) break;
        cursor = slash + 1;
    }

    pthread_mutex_lock(&s_path_cache_mutex);
    int idx = s_path_cache_count % PATH_CACHE_SIZE;
    if (s_path_cache_count < PATH_CACHE_SIZE) s_path_cache_count++;
    strncpy(s_path_cache[idx].in, clean, sizeof(s_path_cache[idx].in) - 1);
    s_path_cache[idx].in[sizeof(s_path_cache[idx].in) - 1] = '\0';
    strncpy(s_path_cache[idx].out, current, sizeof(s_path_cache[idx].out) - 1);
    s_path_cache[idx].out[sizeof(s_path_cache[idx].out) - 1] = '\0';
    pthread_mutex_unlock(&s_path_cache_mutex);

    return s_path_cache[idx].out;
}

static inline int sceIoGetstat(const char *file, SceIoStat *st) {
    struct stat s;
    if (::stat(vita_to_linux_path(file), &s) < 0) return -1;
    if (st) {
        st->st_mode = s.st_mode;
        st->st_size = s.st_size;
        time_t sec = s.st_mtim.tv_sec;
        struct tm tm_val;
        localtime_r(&sec, &tm_val);
        st->st_mtime.year = tm_val.tm_year + 1900;
        st->st_mtime.month = tm_val.tm_mon + 1;
        st->st_mtime.day = tm_val.tm_mday;
        st->st_mtime.hour = tm_val.tm_hour;
        st->st_mtime.minute = tm_val.tm_min;
        st->st_mtime.second = tm_val.tm_sec;
        st->st_mtime.microsecond = 0;
    }
    return 0;
}

#define SCE_SEEK_SET SEEK_SET
#define SCE_SEEK_CUR SEEK_CUR
#define SCE_SEEK_END SEEK_END

#define SCE_AUDIO_VOLUME_0DB 32767
#define SCE_AUDIO_OUT_PORT_TYPE_MAIN 0
#define SCE_AUDIO_OUT_PORT_TYPE_BGM 1
#define SCE_AUDIO_OUT_PORT_TYPE_VOICE 2
#define SCE_AUDIO_OUT_MODE_STEREO 0
#define SCE_AUDIO_VOLUME_FLAG_L_CH 1
#define SCE_AUDIO_VOLUME_FLAG_R_CH 2

// File I/O POSIX shims
static inline SceUID sceIoOpen(const char *file, int flags, int mode) {
    int fd = open(vita_to_linux_path(file), flags, 0666);
    return (fd >= 0) ? fd : -1;
}
static inline int sceIoClose(SceUID fd) { return close(fd); }
static inline int sceIoRead(SceUID fd, void *data, size_t size) { return read(fd, data, size); }
static inline int sceIoWrite(SceUID fd, const void *data, size_t size) { return write(fd, data, size); }
static inline int64_t sceIoLseek(SceUID fd, int64_t offset, int whence) { return lseek(fd, offset, whence); }
static inline ssize_t sceIoPread(SceUID fd, void *data, size_t size, off_t offset) {
    return pread(fd, data, size, offset);
}
static inline int sceIoMkdir(const char *path, int mode) {
    return mkdir(vita_to_linux_path(path), mode);
}
static inline int sceIoRemove(const char *file) {
    return unlink(vita_to_linux_path(file));
}
static inline int sceIoRmdir(const char *path) {
    return rmdir(vita_to_linux_path(path));
}
static inline int sceIoSyncByFd(SceUID fd, int flag) {
    return fsync(fd);
}
#define SCE_S_ISDIR(m) S_ISDIR(m)
typedef off_t SceOff;

// Process Time & Delay
static inline SceUInt64 sceKernelGetProcessTimeWide(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (SceUInt64)ts.tv_sec * 1000000ULL + (ts.tv_nsec / 1000ULL);
}

static inline int sceKernelDelayThread(unsigned int delay_us) {
    return usleep(delay_us);
}

// Mutex shims via pthreads (Tabela global de handles para evitar truncamento em 64 bits)
#define MAX_VITA_MUTEXES 64
extern pthread_mutex_t g_vita_mutex_table[MAX_VITA_MUTEXES];
extern int g_vita_mutex_used[MAX_VITA_MUTEXES];

static inline SceUID sceKernelCreateMutex(const char *name, int attr, int initCount, void *opt) {
    for (int i = 0; i < MAX_VITA_MUTEXES; ++i) {
        if (!g_vita_mutex_used[i]) {
            g_vita_mutex_used[i] = 1;
            pthread_mutex_init(&g_vita_mutex_table[i], NULL);
            return (SceUID)(i + 1);
        }
    }
    return -1;
}
static inline int sceKernelDeleteMutex(SceUID m) {
    int idx = (int)m - 1;
    if (idx >= 0 && idx < MAX_VITA_MUTEXES && g_vita_mutex_used[idx]) {
        pthread_mutex_destroy(&g_vita_mutex_table[idx]);
        g_vita_mutex_used[idx] = 0;
    }
    return 0;
}
static inline int sceKernelLockMutex(SceUID m, int count, void *timeout) {
    int idx = (int)m - 1;
    if (idx >= 0 && idx < MAX_VITA_MUTEXES && g_vita_mutex_used[idx]) {
        pthread_mutex_lock(&g_vita_mutex_table[idx]);
    }
    return 0;
}
static inline int sceKernelUnlockMutex(SceUID m, int count) {
    int idx = (int)m - 1;
    if (idx >= 0 && idx < MAX_VITA_MUTEXES && g_vita_mutex_used[idx]) {
        pthread_mutex_unlock(&g_vita_mutex_table[idx]);
    }
    return 0;
}

// Semaphore shims
#define MAX_VITA_SEMAS 64
extern sem_t g_vita_sema_table[MAX_VITA_SEMAS];
extern int g_vita_sema_used[MAX_VITA_SEMAS];

static inline SceUID sceKernelCreateSema(const char *name, int attr, int initVal, int maxVal, void *opt) {
    for (int i = 0; i < MAX_VITA_SEMAS; ++i) {
        if (!g_vita_sema_used[i]) {
            g_vita_sema_used[i] = 1;
            sem_init(&g_vita_sema_table[i], 0, initVal);
            return (SceUID)(i + 1);
        }
    }
    return -1;
}
static inline int sceKernelDeleteSema(SceUID sema) {
    int idx = (int)sema - 1;
    if (idx >= 0 && idx < MAX_VITA_SEMAS && g_vita_sema_used[idx]) {
        sem_destroy(&g_vita_sema_table[idx]);
        g_vita_sema_used[idx] = 0;
    }
    return 0;
}
static inline int sceKernelSignalSema(SceUID sema, int signal) {
    int idx = (int)sema - 1;
    if (idx >= 0 && idx < MAX_VITA_SEMAS && g_vita_sema_used[idx]) {
        for (int i = 0; i < signal; ++i) sem_post(&g_vita_sema_table[idx]);
    }
    return 0;
}
static inline int sceKernelWaitSema(SceUID sema, int need, void *timeout) {
    int idx = (int)sema - 1;
    if (idx >= 0 && idx < MAX_VITA_SEMAS && g_vita_sema_used[idx]) {
        for (int i = 0; i < need; ++i) sem_wait(&g_vita_sema_table[idx]);
    }
    return 0;
}

#define SCE_KERNEL_CPU_MASK_USER_0 0x01
#define SCE_KERNEL_CPU_MASK_USER_1 0x02
#define SCE_KERNEL_CPU_MASK_USER_2 0x04

// Thread shims via pthreads
#define MAX_VITA_THREADS 32
typedef int (*SceKernelThreadEntry)(size_t args, void *argp);
struct VitaThreadWrapper {
    SceKernelThreadEntry entry;
    void *argp;
    size_t args;
    pthread_t pt;
    int used;
};
extern struct VitaThreadWrapper g_vita_thread_table[MAX_VITA_THREADS];

static inline void *_vita_thread_func(void *p) {
    struct VitaThreadWrapper *w = (struct VitaThreadWrapper *)p;
    w->entry(w->args, w->argp);
    w->used = 0;
    return NULL;
}
static inline SceUID sceKernelCreateThread(const char *name, SceKernelThreadEntry entry, int initPriority, int stackSize, int attr, int cpuAffinityMask, void *opt) {
    for (int i = 0; i < MAX_VITA_THREADS; ++i) {
        if (!g_vita_thread_table[i].used) {
            g_vita_thread_table[i].used = 1;
            g_vita_thread_table[i].entry = entry;
            g_vita_thread_table[i].args = 0;
            g_vita_thread_table[i].argp = NULL;
            return (SceUID)(i + 1);
        }
    }
    return -1;
}
static inline int sceKernelStartThread(SceUID th, size_t args, void *argp) {
    int idx = (int)th - 1;
    if (idx >= 0 && idx < MAX_VITA_THREADS && g_vita_thread_table[idx].used) {
        g_vita_thread_table[idx].args = args;
        g_vita_thread_table[idx].argp = argp;
        pthread_create(&g_vita_thread_table[idx].pt, NULL, _vita_thread_func, &g_vita_thread_table[idx]);
        return 0;
    }
    return -1;
}
static inline int sceKernelExitDeleteThread(int status) {
    pthread_exit(NULL);
    return 0;
}
static inline int sceKernelWaitThreadEnd(SceUID th, int *stat, void *timeout) {
    int idx = (int)th - 1;
    if (idx >= 0 && idx < MAX_VITA_THREADS && g_vita_thread_table[idx].used) {
        pthread_join(g_vita_thread_table[idx].pt, NULL);
        g_vita_thread_table[idx].used = 0;
        return 0;
    }
    return 0;
}

// Power / Clocks no-ops no Linux (controlado por THUG.sh via sysfs)
static inline int scePowerSetArmClockFrequency(int f) { return 0; }
static inline int scePowerSetBusClockFrequency(int f) { return 0; }
static inline int scePowerSetGpuClockFrequency(int f) { return 0; }
static inline int scePowerSetGpuXbarClockFrequency(int f) { return 0; }
static inline int scePowerGetArmClockFrequency(void) { return 1300; }
static inline int scePowerGetBusClockFrequency(void) { return 667; }
static inline int scePowerGetGpuClockFrequency(void) { return 500; }
static inline int scePowerGetGpuXbarClockFrequency(void) { return 333; }

#define SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND 0
#define SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF 1
static inline int sceKernelPowerTick(int type) { return 0; }

// Logging e Clib
#define sceClibPrintf printf

// vitaGL Shims mapeados para SDL2 + GLES2
extern SDL_Window *g_linux_window;
extern SDL_GLContext g_linux_gl_context;

#ifdef __cplusplus
namespace NxVita { extern int g_vita_cadence; }
static inline int get_target_cadence(void) {
    return NxVita::g_vita_cadence;
}
#else
static inline int get_target_cadence(void) {
    return 30;
}
#endif

static inline void vglSwapBuffers(int v) {
    (void)v;
    if (g_linux_window) {
        SDL_GL_SwapWindow(g_linux_window);
    }

    // High-precision frame limiter for Linux KMSDRM:
    // Prevents SoC thermal runaway, stabilizes delta-time in OncePerRender,
    // and eliminates micro-stutters by locking cadence to exactly 33.3ms (30 FPS).
    static SceUInt64 s_last_frame_swap = 0;
    const int cadence = get_target_cadence();
    if (cadence > 0) {
        const SceUInt64 target_us = 1000000ULL / (SceUInt64)cadence;
        SceUInt64 now = sceKernelGetProcessTimeWide();
        if (s_last_frame_swap != 0) {
            SceUInt64 elapsed = now - s_last_frame_swap;
            if (elapsed < target_us) {
                SceUInt64 remaining = target_us - elapsed;
                if (remaining > 3000ULL) {
                    usleep((useconds_t)(remaining - 2000ULL));
                }
                while ((sceKernelGetProcessTimeWide() - s_last_frame_swap) < target_us) {
                    #if defined(__aarch64__)
                    __asm__ __volatile__("yield");
                    #endif
                }
            }
        }
        s_last_frame_swap = sceKernelGetProcessTimeWide();
    }
}

#endif // __LINUX_VITA_SHIM_H__
