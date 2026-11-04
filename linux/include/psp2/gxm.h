#ifndef __PSP2_GXM_H__
#define __PSP2_GXM_H__

#include <stdint.h>
#include <stddef.h>
#include <linux_vita_shim.h>

typedef struct SceGxmTexture SceGxmTexture;
typedef struct SceGxmContext SceGxmContext;
typedef struct SceGxmShaderPatcher SceGxmShaderPatcher;
typedef struct SceGxmProgram SceGxmProgram;
typedef struct SceGxmProgramParameter SceGxmProgramParameter;
typedef struct SceGxmVertexProgram SceGxmVertexProgram;
typedef struct SceGxmFragmentProgram SceGxmFragmentProgram;
typedef struct SceGxmDeferredContext SceGxmDeferredContext;
typedef struct SceGxmCommandList SceGxmCommandList;
typedef struct SceGxmPrecomputedFragmentState SceGxmPrecomputedFragmentState;
typedef struct SceGxmPrecomputedDraw SceGxmPrecomputedDraw;

typedef struct SceGxmVertexAttribute {
    uint16_t streamIndex;
    uint16_t offset;
    uint8_t format;
    uint8_t componentCount;
    uint16_t regIndex;
} SceGxmVertexAttribute;

typedef struct SceGxmVertexStream {
    uint16_t stride;
    uint16_t indexSource;
} SceGxmVertexStream;

typedef struct SceGxmDeferredContextParams {
    void *mem;
    size_t size;
} SceGxmDeferredContextParams;

#define SCE_GXM_REGION_CLIP_OUTSIDE 0
#define SCE_GXM_PRIMITIVE_TRIANGLES 0
#define SCE_GXM_PRIMITIVE_TRIANGLE_STRIP 1
#define SCE_GXM_INDEX_FORMAT_U16 0
#define SCE_GXM_MULTISAMPLE_NONE 0

static inline int sceGxmSetRegionClip(void* ctx, int mode, int x1, int y1, int x2, int y2) { return 0; }
static inline unsigned int sceGxmTextureGetWidth(const SceGxmTexture* t) { return 0; }
static inline unsigned int sceGxmTextureGetHeight(const SceGxmTexture* t) { return 0; }
static inline unsigned int sceGxmTextureGetFormat(const SceGxmTexture* t) { return 0; }

static inline int sceGxmProgramCheck(const SceGxmProgram* p) { return -1; }
static inline int sceGxmShaderPatcherRegisterProgram(void* sp, const SceGxmProgram* p, void** p_id) { return -1; }
static inline const SceGxmProgramParameter* sceGxmProgramFindParameterByName(const SceGxmProgram* p, const char* name) { return NULL; }
static inline unsigned int sceGxmProgramParameterGetResourceIndex(const SceGxmProgramParameter* p) { return 0; }
static inline int sceGxmShaderPatcherCreateVertexProgram(void* sp, void* id, const SceGxmVertexAttribute* att, unsigned int na, const SceGxmVertexStream* vs, unsigned int ns, SceGxmVertexProgram** out) { return -1; }
static inline int sceGxmShaderPatcherCreateFragmentProgram(void* sp, void* id, void* out) { return -1; }
static inline void sceGxmSetUniformDataF(void* b, const SceGxmProgramParameter* p, unsigned int off, unsigned int count, const float* data) {}
static inline void sceGxmSetPrecomputedVertexState(void* ctx, void* s) {}
static inline void sceGxmSetPrecomputedFragmentState(void* ctx, void* s) {}
static inline void sceGxmSetFragmentProgram(void* ctx, void* fp) {}
static inline void sceGxmSetVertexProgram(void* ctx, void* vp) {}
static inline void sceGxmReserveVertexDefaultUniformBuffer(void* ctx, void** vb) { if (vb) *vb = NULL; }
static inline void sceGxmReserveFragmentDefaultUniformBuffer(void* ctx, void** fb) { if (fb) *fb = NULL; }
static inline unsigned int sceGxmGetPrecomputedFragmentStateSize(void* fp) { return 0; }
static inline unsigned int sceGxmGetPrecomputedDrawSize(void* vp) { return 0; }
static inline unsigned int sceGxmProgramGetDefaultUniformBufferSize(const SceGxmProgram* p) { return 0; }
static inline int sceGxmPrecomputedFragmentStateInit(void* fs, void* fp, void* mem) { return -1; }
static inline void sceGxmPrecomputedFragmentStateSetTexture(void* fs, int unit, const SceGxmTexture* tex) {}
static inline void sceGxmPrecomputedFragmentStateSetDefaultUniformBuffer(void* fs, void* ub) {}
static inline int sceGxmPrecomputedDrawInit(void* dr, void* vp, void* mem) { return -1; }
static inline void sceGxmPrecomputedDrawSetAllVertexStreams(void* dr, const void** flux) {}
static inline void sceGxmPrecomputedDrawSetParams(void* dr, int prim, int fmt, unsigned int count, const void* idx) {}
static inline void sceGxmDrawPrecomputed(void* ctx, void* dr) {}
static inline void sceGxmSetFragmentTexture(void* ctx, int unit, const SceGxmTexture* tex) {}
static inline void sceGxmSetVertexStream(void* ctx, unsigned int stream, const void* data) {}
static inline void sceGxmDraw(void* ctx, int prim, int fmt, unsigned int count, const void* idx) {}
static inline int sceGxmCreateDeferredContext(const void* prm, void** ctx) { return -1; }
static inline int sceGxmBeginCommandList(void* ctx) { return -1; }
static inline int sceGxmEndCommandList(void* ctx, void* liste) { return -1; }
static inline void sceGxmExecuteCommandList(void* ctx, void* liste) {}

#endif // __PSP2_GXM_H__
