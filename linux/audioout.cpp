#include <SDL.h>
#include <psp2/audioout.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_AUDIO_PORTS 8

struct AudioPort {
    bool in_use;
    volatile bool flushed;
    int type;
    int len;
    int freq;
    int32_t vol_l;
    int32_t vol_r;
    int max_queued_bytes;
    SDL_AudioStream *stream;
    SDL_mutex *mutex;
    SDL_cond *cond;
};

static AudioPort g_ports[MAX_AUDIO_PORTS];
static SDL_AudioDeviceID g_audio_device = 0;
static SDL_mutex *g_audio_global_mutex = NULL;
static bool g_audio_device_attempted = false;

static void AudioMixerCallback(void *userdata, Uint8 *stream, int len)
{
    (void)userdata;
    int frames = len / 4; // 16-bit stereo = 4 bytes per frame
    memset(stream, 0, len);

    // 32-bit mixing buffer to prevent clipping during sample accumulation
    int32_t mix_buffer[2048 * 2];
    if (frames * 2 > (int)(sizeof(mix_buffer) / sizeof(mix_buffer[0])))
        frames = (sizeof(mix_buffer) / sizeof(mix_buffer[0])) / 2;

    memset(mix_buffer, 0, frames * 2 * sizeof(int32_t));

    for (int p = 0; p < MAX_AUDIO_PORTS; p++)
    {
        AudioPort *port = &g_ports[p];
        if (!port->in_use || !port->stream || !port->mutex)
            continue;

        SDL_LockMutex(port->mutex);

        int16_t temp[2048 * 2];
        int bytes_wanted = frames * 4;
        int bytes_got = SDL_AudioStreamGet(port->stream, temp, bytes_wanted);

        if (bytes_got > 0)
        {
            int got_samples = bytes_got / 2;
            int32_t vl = port->vol_l;
            int32_t vr = port->vol_r;

            for (int i = 0; i < got_samples; i += 2)
            {
                // Volume scaling (0..32767 where 32767 = 1.0)
                mix_buffer[i]     += (int32_t)((temp[i]     * vl) >> 15);
                mix_buffer[i + 1] += (int32_t)((temp[i + 1] * vr) >> 15);
            }

            // Wake up producer thread if queue has drained below threshold
            if (port->cond)
            {
                SDL_CondSignal(port->cond);
            }
        }

        SDL_UnlockMutex(port->mutex);
    }

    // Convert 32-bit accumulated samples to 16-bit clamped PCM
    int16_t *out = (int16_t *)stream;
    for (int i = 0; i < frames * 2; i++)
    {
        int32_t val = mix_buffer[i];
        if (val > 32767) val = 32767;
        else if (val < -32768) val = -32768;
        out[i] = (int16_t)val;
    }
}

static void EnsureGlobalAudioInit()
{
    if (!g_audio_global_mutex)
    {
        g_audio_global_mutex = SDL_CreateMutex();
    }
}

extern "C" int sceAudioOutOpenPort(int type, int len, int freq, int mode)
{
    (void)mode;
    EnsureGlobalAudioInit();
    SDL_LockMutex(g_audio_global_mutex);

    if (!g_audio_device_attempted)
    {
        g_audio_device_attempted = true;

        SDL_AudioSpec desired, obtained;
        SDL_zero(desired);
        desired.freq = 48000;
        desired.format = AUDIO_S16SYS;
        desired.channels = 2;
        desired.samples = 512;
        desired.callback = AudioMixerCallback;
        desired.userdata = NULL;

        g_audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
        if (g_audio_device > 0)
        {
            printf("[AUDIO] SDL_OpenAudioDevice opened (device %u, %d Hz, %d channels, %d samples)\n",
                   g_audio_device, obtained.freq, obtained.channels, obtained.samples);
            fflush(stdout);
            SDL_PauseAudioDevice(g_audio_device, 0);
        }
        else
        {
            printf("[AUDIO] SDL_OpenAudioDevice unavailable: %s (falling back to timed pacing)\n",
                   SDL_GetError());
            fflush(stdout);
        }
    }

    int slot = -1;
    for (int i = 0; i < MAX_AUDIO_PORTS; i++)
    {
        if (!g_ports[i].in_use)
        {
            slot = i;
            break;
        }
    }

    if (slot < 0)
    {
        printf("[AUDIO] Error: maximum audio ports exceeded!\n");
        fflush(stdout);
        SDL_UnlockMutex(g_audio_global_mutex);
        return -1;
    }

    AudioPort *p = &g_ports[slot];
    p->in_use = true;
    p->flushed = false;
    p->type = type;
    p->len = len > 0 ? len : 512;
    p->freq = freq > 0 ? freq : 48000;
    p->vol_l = 32767;
    p->vol_r = 32767;

    // Resample from port frequency (e.g. 44100 / 22050 / 48000) to 48000 Hz stereo S16
    p->stream = SDL_NewAudioStream(AUDIO_S16SYS, 2, p->freq, AUDIO_S16SYS, 2, 48000);
    p->mutex = SDL_CreateMutex();
    p->cond = SDL_CreateCond();

    // Fine-grained buffer threshold (~2 grains ahead, min 4096 bytes = ~21 ms at 48 kHz)
    int frames_48k = (p->len * 48000) / p->freq;
    int grain_bytes_48k = frames_48k * 4;
    p->max_queued_bytes = (grain_bytes_48k * 2 > 4096) ? (grain_bytes_48k * 2) : 4096;

    printf("[AUDIO] Port %d opened (type=%d, grain=%d, freq=%d Hz, max_queue=%d bytes)\n",
           slot + 1, type, p->len, p->freq, p->max_queued_bytes);
    fflush(stdout);

    SDL_UnlockMutex(g_audio_global_mutex);
    return slot + 1;
}

extern "C" int sceAudioOutReleasePort(int port)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS)
        return -1;

    EnsureGlobalAudioInit();
    SDL_LockMutex(g_audio_global_mutex);

    AudioPort *p = &g_ports[idx];
    if (p->in_use)
    {
        if (p->mutex) SDL_LockMutex(p->mutex);
        p->in_use = false;
        if (p->cond) SDL_CondBroadcast(p->cond);
        if (p->mutex) SDL_UnlockMutex(p->mutex);

        if (p->stream)
        {
            SDL_FreeAudioStream(p->stream);
            p->stream = NULL;
        }
        if (p->cond)
        {
            SDL_DestroyCond(p->cond);
            p->cond = NULL;
        }
        if (p->mutex)
        {
            SDL_DestroyMutex(p->mutex);
            p->mutex = NULL;
        }
        printf("[AUDIO] Port %d released\n", port);
        fflush(stdout);
    }

    SDL_UnlockMutex(g_audio_global_mutex);
    return 0;
}

extern "C" int sceAudioOutSetVolume(int port, int flag, int *vol)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS || !vol)
        return -1;

    AudioPort *p = &g_ports[idx];
    if (!p->in_use || !p->mutex)
        return -1;

    SDL_LockMutex(p->mutex);
    if (flag & SCE_AUDIO_VOLUME_FLAG_L_CH)
        p->vol_l = vol[0];
    if (flag & SCE_AUDIO_VOLUME_FLAG_R_CH)
        p->vol_r = vol[1];
    SDL_UnlockMutex(p->mutex);

    return 0;
}

extern "C" int sceAudioOutSetConfig(int port, int len, int freq, int mode)
{
    (void)mode;
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS)
        return -1;

    AudioPort *p = &g_ports[idx];
    if (!p->in_use || !p->mutex)
        return -1;

    SDL_LockMutex(p->mutex);
    if (len > 0)
        p->len = len;
    if (freq > 0 && freq != p->freq)
    {
        p->freq = freq;
        if (p->stream)
            SDL_FreeAudioStream(p->stream);
        p->stream = SDL_NewAudioStream(AUDIO_S16SYS, 2, p->freq, AUDIO_S16SYS, 2, 48000);
    }
    int frames_48k = (p->len * 48000) / p->freq;
    int grain_bytes_48k = frames_48k * 4;
    p->max_queued_bytes = (grain_bytes_48k * 2 > 4096) ? (grain_bytes_48k * 2) : 4096;
    p->flushed = false;
    SDL_UnlockMutex(p->mutex);
    return 0;
}

extern "C" int sceAudioOutFlushPort(int port)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS)
        return -1;

    AudioPort *p = &g_ports[idx];
    if (!p->in_use || !p->mutex)
        return -1;

    SDL_LockMutex(p->mutex);
    p->flushed = true;
    if (p->stream)
        SDL_AudioStreamClear(p->stream);
    if (p->cond)
        SDL_CondBroadcast(p->cond);
    SDL_UnlockMutex(p->mutex);
    return 0;
}

extern "C" int sceAudioOutOutputSamples(int port, const void *buf, int num_samples)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS || !buf || num_samples <= 0)
        return -1;

    AudioPort *p = &g_ports[idx];
    if (!p->in_use)
        return -1;

    // Emulate hardware audio clock timing if no sound hardware is present
    if (g_audio_device == 0)
    {
        int duration_ms = (num_samples * 1000) / p->freq;
        if (duration_ms < 1) duration_ms = 1;
        SDL_Delay(duration_ms);
        return 0;
    }

    const int step = (p->len > 0 && p->len <= 512) ? p->len : 512;
    const uint8_t *ptr = (const uint8_t *)buf;
    int remaining = num_samples;

    SDL_LockMutex(p->mutex);
    p->flushed = false;

    while (remaining > 0 && p->in_use && !p->flushed)
    {
        int slice = (remaining > step) ? step : remaining;
        SDL_AudioStreamPut(p->stream, ptr, slice * 4);
        ptr += slice * 4;
        remaining -= slice;

        int timeouts = 0;
        while (p->in_use && !p->flushed && SDL_AudioStreamAvailable(p->stream) > p->max_queued_bytes)
        {
            if (SDL_CondWaitTimeout(p->cond, p->mutex, 15) == SDL_MUTEX_TIMEDOUT)
            {
                if (++timeouts >= 4)
                    break;
            }
        }
    }

    SDL_UnlockMutex(p->mutex);
    return 0;
}

extern "C" int sceAudioOutOutput(int port, const void *buf)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS)
        return -1;
    AudioPort *p = &g_ports[idx];
    return sceAudioOutOutputSamples(port, buf, p->len);
}

extern "C" int sceAudioOutGetRestSample(int port)
{
    int idx = port - 1;
    if (idx < 0 || idx >= MAX_AUDIO_PORTS)
        return 0;

    AudioPort *p = &g_ports[idx];
    if (!p->in_use || !p->stream || !p->mutex)
        return 0;

    SDL_LockMutex(p->mutex);
    int bytes = SDL_AudioStreamAvailable(p->stream);
    SDL_UnlockMutex(p->mutex);

    int samples_48k = bytes / 4;
    return (int)(((int64_t)samples_48k * p->freq) / 48000);
}
