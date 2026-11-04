#ifndef __PSP2_AUDIOOUT_H__
#define __PSP2_AUDIOOUT_H__

#include <linux_vita_shim.h>

#ifndef SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO
#define SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

int sceAudioOutOpenPort(int type, int len, int freq, int mode);
int sceAudioOutReleasePort(int port);
int sceAudioOutSetVolume(int port, int flag, int *vol);
int sceAudioOutSetConfig(int port, int len, int freq, int mode);
int sceAudioOutFlushPort(int port);
int sceAudioOutOutput(int port, const void *buf);
int sceAudioOutOutputSamples(int port, const void *buf, int num_samples);
int sceAudioOutGetRestSample(int port);

#ifdef __cplusplus
}
#endif

#endif // __PSP2_AUDIOOUT_H__
