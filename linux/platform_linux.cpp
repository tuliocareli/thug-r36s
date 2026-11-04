#include "linux_vita_shim.h"

#include <psp2/gxm.h>
#include <psp2/ctrl.h>
#include <GL/gl.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <execinfo.h>

static void early_crash_handler(int sig, siginfo_t *info, void *ucontext) {
    char buf[256];
    int len = snprintf(buf, sizeof(buf), "\n[FATAL CRASH] Signal %d at address %p\n--- STACK TRACE ---\n", sig, info ? info->si_addr : NULL);
    if (len > 0) (void)write(STDERR_FILENO, buf, len);
    void *callstack[64];
    int frames = backtrace(callstack, 64);
    backtrace_symbols_fd(callstack, frames, STDERR_FILENO);
    (void)write(STDERR_FILENO, "\n--- /proc/self/maps ---\n", 25);
    int fd = open("/proc/self/maps", O_RDONLY);
    if (fd >= 0) {
        char mapbuf[1024];
        int n;
        while ((n = read(fd, mapbuf, sizeof(mapbuf))) > 0) {
            (void)write(STDERR_FILENO, mapbuf, n);
        }
        close(fd);
    }
    _exit(128 + sig);
}

__attribute__((constructor(101)))
static void early_crash_init(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = early_crash_handler;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    fprintf(stderr, "[THUG_EARLY_INIT] Early crash handler installed!\n");
    fflush(stderr);
}

pthread_mutex_t g_vita_mutex_table[MAX_VITA_MUTEXES];
int g_vita_mutex_used[MAX_VITA_MUTEXES] = {0};
sem_t g_vita_sema_table[MAX_VITA_SEMAS];
int g_vita_sema_used[MAX_VITA_SEMAS] = {0};
struct VitaThreadWrapper g_vita_thread_table[MAX_VITA_THREADS];

SDL_Window *g_linux_window = NULL;
SDL_GLContext g_linux_gl_context = NULL;

SceGxmContext *gxm_context = NULL;
SceGxmShaderPatcher *gxm_shader_patcher = NULL;
GLboolean is_rendering_display = GL_TRUE;
uint32_t vgl_framecount = 0;

extern "C" void vita_vgl_init_tot(void) {
    if (g_linux_window) return;

    printf("[LINUX] vita_vgl_init_tot: initializing SDL2...\n");
    fflush(stdout);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("[LINUX] Failed to init SDL: %s\n", SDL_GetError());
        fflush(stdout);
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    int win_w = 640;
    int win_h = 480;
    const char *w_env = getenv("THUG_WIDTH");
    const char *h_env = getenv("THUG_HEIGHT");
    if (w_env && atoi(w_env) > 0) win_w = atoi(w_env);
    if (h_env && atoi(h_env) > 0) win_h = atoi(h_env);

    printf("[LINUX] Creating %dx%d OpenGL window...\n", win_w, win_h);
    fflush(stdout);

    g_linux_window = SDL_CreateWindow(
        "Tony Hawk's Underground (R36S)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_w, win_h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!g_linux_window) {
        printf("[LINUX] Failed to create SDL Window: %s\n", SDL_GetError());
        fflush(stdout);
        return;
    }

    printf("[LINUX] Creating OpenGL context...\n");
    fflush(stdout);

    g_linux_gl_context = SDL_GL_CreateContext(g_linux_window);
    if (!g_linux_gl_context) {
        printf("[LINUX] Failed to create GL Context: %s\n", SDL_GetError());
        fflush(stdout);
        return;
    }

    const char *vsync_env = getenv("THUG_VSYNC");
    // Default to 1 (standard 60Hz vblank interval) with frame limiter to prevent brutal 15 FPS drops on KMSDRM
    int vsync = (vsync_env != NULL) ? atoi(vsync_env) : 1;
    SDL_GL_SetSwapInterval(vsync);
    printf("[LINUX] SDL2 + GLES 2.0 KMSDRM Initialized (%dx%d, vsync=%d)\n", win_w, win_h, vsync);
    fflush(stdout);
}

extern "C" void vita_ecran_boot_tot(void) {}
extern "C" int vita_ecran_boot(void) { return 0; }
extern "C" void vita_dbgsrv_start(void) {}
extern "C" void vita_log_start_watchdog(void) {}

// Runtime globals
bool g_hasJustEnteredNetworkGame = false;

// Glspy / memspy stubs
extern "C" {
    int vita_glspy_ouverte = 0;
    int vita_glspy_phase = 0;
    void vita_glspy_bilan(void) {}
    void vita_memspy_niveau(const char *nom) {}
}

// GXM stubs for Linux (falls back to standard OpenGL rendering pipeline)
namespace NxVita {
    struct SShaderMateriau;
    bool g_vita_vpr = false;
    bool g_vita_gxm_direct = false;
    int g_vita_gp_reutil[2] = { 0, 0 };

    int GxmFamilleMelange(unsigned int blend, bool translucide) {
        return 0;
    }

    void GxmMateriauDebut(const float *mvp) {}

    bool GxmMateriauLot(const SShaderMateriau *p, SceGxmTexture *const *p_tex,
                         const int *p_first, const int *p_count, int n_plages,
                         unsigned int ibo, int famille) {
        return false;
    }
}

static SDL_GameController *g_gamepad = NULL;

extern "C" int sceCtrlPeekBufferPositiveExt2(int port, SceCtrlData *pad, int count) {
    if (!pad || count <= 0) return -1;
    memset(pad, 0, sizeof(SceCtrlData) * count);

    SDL_PumpEvents();
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            _exit(0);
        }
    }

    if (!g_gamepad && SDL_NumJoysticks() > 0) {
        g_gamepad = SDL_GameControllerOpen(0);
        if (g_gamepad) {
            printf("[LINUX] Opened GameController: %s\n", SDL_GameControllerName(g_gamepad));
        }
    }

    uint32_t buttons = 0;
    int lx = 128, ly = 128, rx = 128, ry = 128;

    if (g_gamepad) {
        // R36S Layout 1:1 PS2:
        // Botao B (baixo) = Cruz (PS2) = Ollie
        // Botao A (direita) = Circulo (PS2) = Grab
        // Botao Y (esquerda) = Quadrado (PS2) = Flip
        // Botao X (cima) = Triangulo (PS2) = Grind
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_B)) buttons |= SCE_CTRL_CROSS;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_A)) buttons |= SCE_CTRL_CIRCLE;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_Y)) buttons |= SCE_CTRL_SQUARE;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_X)) buttons |= SCE_CTRL_TRIANGLE;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) buttons |= SCE_CTRL_L1;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) buttons |= SCE_CTRL_R1;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_START)) buttons |= SCE_CTRL_START;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_BACK)) buttons |= SCE_CTRL_SELECT;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP)) buttons |= SCE_CTRL_UP;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) buttons |= SCE_CTRL_DOWN;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) buttons |= SCE_CTRL_LEFT;
        if (SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) buttons |= SCE_CTRL_RIGHT;

        if (SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 8000) buttons |= SCE_CTRL_L2;
        if (SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 8000) buttons |= SCE_CTRL_R2;

        int raw_lx = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_LEFTX);
        int raw_ly = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_LEFTY);
        int raw_rx = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_RIGHTX);
        int raw_ry = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_RIGHTY);

        lx = ((raw_lx + 32768) >> 8) & 0xFF;
        ly = ((raw_ly + 32768) >> 8) & 0xFF;
        rx = ((raw_rx + 32768) >> 8) & 0xFF;
        ry = ((raw_ry + 32768) >> 8) & 0xFF;
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys) {
        if (keys[SDL_SCANCODE_RETURN]) buttons |= SCE_CTRL_START;
        if (keys[SDL_SCANCODE_ESCAPE]) buttons |= SCE_CTRL_SELECT;
        if (keys[SDL_SCANCODE_SPACE])  buttons |= SCE_CTRL_CROSS;
        if (keys[SDL_SCANCODE_C])      buttons |= SCE_CTRL_CIRCLE;
        if (keys[SDL_SCANCODE_X])      buttons |= SCE_CTRL_SQUARE;
        if (keys[SDL_SCANCODE_Z])      buttons |= SCE_CTRL_TRIANGLE;
        if (keys[SDL_SCANCODE_Q])      buttons |= SCE_CTRL_L1;
        if (keys[SDL_SCANCODE_E])      buttons |= SCE_CTRL_R1;
        if (keys[SDL_SCANCODE_UP])     buttons |= SCE_CTRL_UP;
        if (keys[SDL_SCANCODE_DOWN])   buttons |= SCE_CTRL_DOWN;
        if (keys[SDL_SCANCODE_LEFT])   buttons |= SCE_CTRL_LEFT;
        if (keys[SDL_SCANCODE_RIGHT])  buttons |= SCE_CTRL_RIGHT;
    }

    for (int i = 0; i < count; ++i) {
        pad[i].buttons = buttons;
        pad[i].lx = (uint8_t)lx;
        pad[i].ly = (uint8_t)ly;
        pad[i].rx = (uint8_t)rx;
        pad[i].ry = (uint8_t)ry;
    }

    return count;
}


