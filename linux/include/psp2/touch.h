#ifndef __PSP2_TOUCH_H__
#define __PSP2_TOUCH_H__

#include <stdint.h>
#include <linux_vita_shim.h>

#define SCE_TOUCH_PORT_FRONT 0
#define SCE_TOUCH_PORT_BACK 1
#define SCE_TOUCH_SAMPLING_STATE_START 1

typedef struct SceTouchReport {
    uint8_t id;
    uint8_t force;
    int16_t x;
    int16_t y;
    uint8_t reserved[8];
    uint16_t info;
} SceTouchReport;

typedef struct SceTouchData {
    uint64_t timeStamp;
    uint32_t status;
    uint32_t reportNum;
    SceTouchReport report[8];
} SceTouchData;

typedef struct SceTouchPanelInfo {
    int16_t minAaX;
    int16_t minAaY;
    int16_t maxAaX;
    int16_t maxAaY;
    int16_t minDispX;
    int16_t minDispY;
    int16_t maxDispX;
    int16_t maxDispY;
    uint8_t minForce;
    uint8_t maxForce;
    uint8_t reserved[30];
} SceTouchPanelInfo;

static inline int sceTouchSetSamplingState(int port, int state) { return 0; }
static inline int sceTouchGetPanelInfo(int port, SceTouchPanelInfo *info) {
    if (info) {
        info->minAaX = 0;
        info->maxAaX = 1920;
        info->minAaY = 0;
        info->maxAaY = 1088;
    }
    return 0;
}
static inline int sceTouchPeek(int port, SceTouchData *data, int count) {
    if (data) {
        data->reportNum = 0;
    }
    return 0;
}

#endif // __PSP2_TOUCH_H__
