#ifndef __PSP2_CTRL_H__
#define __PSP2_CTRL_H__

#include <stdint.h>
#include <string.h>
#include <linux_vita_shim.h>

#define SCE_CTRL_SELECT      0x00000001
#define SCE_CTRL_L3          0x00000002
#define SCE_CTRL_R3          0x00000004
#define SCE_CTRL_START       0x00000008
#define SCE_CTRL_UP          0x00000010
#define SCE_CTRL_RIGHT       0x00000020
#define SCE_CTRL_DOWN        0x00000040
#define SCE_CTRL_LEFT        0x00000080
#define SCE_CTRL_L1          0x00000100
#define SCE_CTRL_R1          0x00000200
#define SCE_CTRL_L2          0x00000400
#define SCE_CTRL_R2          0x00000800
#define SCE_CTRL_TRIANGLE    0x00001000
#define SCE_CTRL_CIRCLE      0x00002000
#define SCE_CTRL_CROSS       0x00004000
#define SCE_CTRL_SQUARE      0x00008000

#define SCE_CTRL_MODE_ANALOG_WIDE 1

typedef struct SceCtrlData {
    uint64_t timeStamp;
    uint32_t buttons;
    uint8_t lx;
    uint8_t ly;
    uint8_t rx;
    uint8_t ry;
    uint8_t up;
    uint8_t right;
    uint8_t down;
    uint8_t left;
    uint8_t lt;
    uint8_t rt;
    uint8_t reserved[16];
} SceCtrlData;

static inline int sceCtrlSetSamplingMode(int mode) { return 0; }
static inline int sceCtrlSetSamplingModeExt(int mode) { return 0; }
#ifdef __cplusplus
extern "C" {
#endif
int sceCtrlPeekBufferPositiveExt2(int port, SceCtrlData *pad, int count);
#ifdef __cplusplus
}
#endif

#endif // __PSP2_CTRL_H__
