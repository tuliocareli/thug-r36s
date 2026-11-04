#ifndef VITA_LOG_H
#define VITA_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void vita_log_init(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);
}
static inline void vita_log_shutdown(void) { fflush(stdout); fflush(stderr); }
static inline void vita_log_start_watchdog(void) {}
static inline void vita_log_flush(void) { fflush(stdout); fflush(stderr); }

static inline void vita_log_printf(const char *sys, const char *fmt, ...) {
    static int s_checked = 0;
    static int s_enabled = 0;
    if (!s_checked) {
        const char *env = getenv("THUG_VERBOSE");
        s_enabled = (env && env[0] && env[0] != '0') ? 1 : 0;
        s_checked = 1;
    }
    if (!s_enabled)
        return;
    va_list args;
    printf("[%s] ", sys ? sys : "LOG");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

static inline int vita_log_ring_read(unsigned int *pos, char *out, int max) { return 0; }

#define VLOG(sys, ...) vita_log_printf(sys, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // VITA_LOG_H
