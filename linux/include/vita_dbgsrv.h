#ifndef VITA_DBGSRV_H
#define VITA_DBGSRV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VitaDbgPad {
    unsigned int buttons;
    int has_sticks;
    unsigned char lx, ly, rx, ry;
} VitaDbgPad;

static inline void vita_dbgsrv_start(void) {}
static inline void vita_dbgsrv_stop(void) {}
static inline void vita_dbgsrv_frame(void) {}
static inline int vita_dbgsrv_take_command(char *buf, size_t sz) { return 0; }
static inline void vita_dbgsrv_set_busy(int b) {}
static inline int vita_dbgsrv_pad(VitaDbgPad *pad) { return 0; }

#ifdef __cplusplus
}
#endif

#endif // VITA_DBGSRV_H
