#ifndef __VITAGL_SHIM_H__
#define __VITAGL_SHIM_H__

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <psp2/gxm.h>
#include <linux_vita_shim.h>

#ifndef GL_CG_VERTEX_SHADER_EXT
#define GL_CG_VERTEX_SHADER_EXT GL_VERTEX_SHADER
#endif
#ifndef GL_CG_FRAGMENT_SHADER_EXT
#define GL_CG_FRAGMENT_SHADER_EXT GL_FRAGMENT_SHADER
#endif

#ifndef VGL_MEM_RAM
#define VGL_MEM_RAM 0
#define VGL_MEM_VRAM 1
#endif
typedef int vglMemType;

static inline void vglEnd() {}
static inline void* vglGetGxmTexture(GLenum target) { return NULL; }
static inline void vglGetShaderBinary(GLuint sh, size_t max, GLsizei* len, void* buf) { if(len) *len = 0; }
static inline void* vglGetTexDataPointer(GLenum target) { return NULL; }
static inline int vglInit(int a) { return 1; }
static inline int vglInitExtended(int a, int b, int c, int d, int e) { return 1; }
static inline void vglLazyFree(void* p) { free(p); }
static inline size_t vglMemFree(int type) { return 64*1024*1024; }
static inline void* vglReserveVertexUniformBuffer(size_t s) { return NULL; }
static inline void vglSetFragmentBufferSize(size_t s) {}
static inline void vglSetParamBufferSize(size_t s) {}
static inline void vglSetVDMBufferSize(size_t s) {}
static inline void vglSetVertexBufferSize(size_t s) {}
static inline void vglUseCachedMem(int a) {}
static inline void* vgl_fast_memcpy(void* dst, const void* src, size_t n) { return memcpy(dst, src, n); }
static inline void vgl_fill_uniform_data(void* a, void* b, void* c) {}
static inline void vgl_free(void* p) { free(p); }
#ifdef __cplusplus
extern "C" {
#endif
static inline void* vgl_memalign(size_t alignment, size_t size, vglMemType type) {
    void* p = NULL;
    if (posix_memalign(&p, alignment, size) != 0) return malloc(size);
    return p;
}
static inline uint8_t* vgl_reserve_data_pool(uint32_t s) { return NULL; }
#ifdef __cplusplus
}
#endif

#endif // __VITAGL_SHIM_H__
