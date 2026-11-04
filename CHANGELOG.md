# Changelog

All notable changes to the **THUG-R36S** port are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased] - Work In Progress (WIP)

### 🚀 Performance & Architecture Optimizations (RK3326 / Mali-G31)
* **16-bit Texture Pipeline (`LIBGL_FORCE16BITS=1`):** Enforced dynamic conversion of 32-bit RGBA textures to 16-bit (RGB565 / RGBA4444) in VRAM via GL4ES, reducing memory bandwidth pressure by 50% on the Mali-G31 MP2.
* **CPU Particle Elimination (`g_vita_particules = 0` & `g_vita_prs = 0`):** Cut parametric and legacy CPU particle generation loops, saving over 8.4 ms of CPU time per frame during grinds, bails, and splashes.
* **Selective Pedestrian Shadow Culling:** Suppressed per-frame `Ped_Shadow.mdl` matrix updates and draw calls for 30–50 background NPCs in `shadowcomponent.cpp`, while preserving the main skater shadow.
* **Vehicle Wheel Shadow Culling:** Disabled dynamic car wheel shadows and raycast traces on Linux (`m_shadowEnabled = false`).
* **Static Billboard & VCW Bypass:** Replaced per-frame CPU vertex re-writes for trees and neon signs (`g_vita_billboards = false`, `g_vita_vcw = false`) with static VBO coordinates.
* **Thread Contention Fix:** Disabled multi-threaded sector list creation semaphores on Linux (`g_vita_listes_mt = false`), eliminating context-switch stalls on in-order Cortex-A35 cores.
* **Hardware VSync & Frame Pacing:** Configured KMSDRM swap intervals (`LIBGL_VSYNC=1`, `THUG_VSYNC=1`) and adjusted `OncePerRender()` delta smoothing to operate cleanly at native 60 FPS without judder or physics acceleration jumps.
* **Single-Bone Skinning Fast-Path:** Accelerated CPU vertex transformations for rigid single-bone meshes by bypassing float unpacking and 3-bone weight loops for over 70% of character vertices.

### 🐛 Visual & Graphics Fixes
* **Punch-Through Alpha Test (CAS Head Hole Fix):** Enabled `GL_ALPHA_TEST` (greater than 0.05f) during the opaque skinning pass in `DrawSkinned`. Transparent hair and accessory texels no longer write depth into the Z-buffer, eliminating the transparent vacuum hole behind CAS skaters.
* **NPC Body Parts & CAS Texture Swapping:** Fixed GL texture handle ownership in `plat_replace_texture` (`p_new->m_gl_texture = 0`), preventing temporary texture objects from triggering `glDeleteTextures` and leaving skater limbs invisible.
* **Model Material Refresh:** Implemented `CVitaModel::plat_refresh_materials()` to dynamically bind new texture IDs to GPU meshes whenever textures are replaced.
* **LevelObject Cloned Instances (Tombstones & Gates):** Added proper modelview matrix stacking (`glPushMatrix` / `glMultMatrixf`) and GL state restoration in `dessiner_instances()`, restoring cemetery tombstones, chainlink fences, and park gates.
* **Skybox Rendering:** Configured sky dome to render at infinite projection depth (`0.99999f * w`) with `GL_LEQUAL` depth testing, preventing horizon geometry and models from being clipped by the sky.
* **Multi-Texture Pool & Water Shading:** Restored fixed-function layer blend modes (`GL_ADD` for highlights, `GL_MODULATE` for shadows) and corrected layer 2 texture pointer offsets.

### 🎬 Media & Platform Integration
* **Bink Video Decoding (FMV):** Integrated FFmpeg Bink video (.bik) decoder with synchronized stereo PCM audio streaming in `p_movies.cpp` and `vita_bink.c`.
* **ALSA / OpenAL Audio Backend:** Implemented background soundtrack streaming and skater SFX with volume balance control and channel unmuting in launcher scripts.
* **SDL2 KMSDRM Driver:** Added direct KMSDRM framebuffer rendering without X11 or Wayland overhead, matching the native 640x480 resolution of the R36S display.
* **PortMaster Launcher & Gamepad:** Crafted launch script `THUG.sh` with CPU/GPU performance governor triggers, process nice priority elevation, safety swap allocation, and complete `gptokeyb` mappings.

---

## [0.1.0-vita] - Initial PlayStation Vita Baseline
* Initial proof-of-concept Xbox format parser and VitaGL backend by `iwannagooutside`.
