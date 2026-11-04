# THUG-R36S Backlog & Roadmap

This document outlines planned improvements, known open issues, and feature requests for the **THUG-R36S** port.
Contributors are encouraged to pick any of these items and submit Pull Requests!

---

## 🎯 High Priority (Core Experience)

### 1. Lightweight GLES 2.0 Shader Particles
* **Current Status:** CPU particle loops (`p_NxParticle.cpp` and `p_NxNewParticle.cpp`) are currently bypassed on Linux (`g_vita_prs = 0`, `g_vita_particules = 0`) to prevent the in-order Cortex-A35 from dropping to 15–20 FPS during rail grinds.
* **Goal:** Write a minimal GLES 2.0 vertex shader that calculates particle quad orientation (`billboard`) on the GPU using uniform registers, restoring grind sparks, blood, and wheel splashes at 60 FPS.
* **Files involved:** `Code/Gfx/Vita/p_NxParticle.cpp`, `Code/Gfx/Vita/p_NxNewParticle.cpp`, `Code/Gfx/Vita/p_shader_decor.cpp`.

### 2. On-Screen Virtual Keyboard (OSK)
* **Current Status:** Entering names in Create-A-Skater, Create-A-Park, or high score records expects either direct keyboard input or game controller alphanumeric cycling.
* **Goal:** Implement a lightweight, clean gamepad-driven on-screen keyboard overlay (D-Pad + A/B selection) when a text input prompt is triggered.
* **Files involved:** `Code/Gel/Scripting/cfuncs.cpp`, `linux/platform_linux.cpp`.

### 3. PortMaster Official Packaging
* **Current Status:** The port runs directly via `ports/THUG.sh` on SD card.
* **Goal:** Finalize the official `port.json` metadata specification, create a 2:1 aspect ratio title boxart (`cover.png`) and screenshot (`screenshot.png`), and prepare the PR for the [PortMaster-New](https://github.com/PortsMaster/PortMaster-New) repository.

---

## 🔧 Medium Priority (Graphics & Polish)

### 4. Instance Batching for LevelObjects
* **Current Status:** Cloned level objects (cemetery tombstones, chainlink gates, benches) are drawn sequentially with `glPushMatrix` / `glMultMatrixf`.
* **Goal:** Group instances sharing identical textures and shaders into indexed batches to further reduce draw calls on dense levels like Manhattan and Moscow.
* **Files involved:** `Code/Gfx/Vita/p_world_render.cpp` (`dessiner_instances`).

### 5. Environment Reflection Mapping
* **Current Status:** Water, glossy floors, and car hoods use standard multi-pass texture blending (`GL_ADD` / `GL_MODULATE`).
* **Goal:** Add hardware spherical reflection coordinates generation in GLES 2.0 shaders for metallic surfaces.
* **Files involved:** `Code/Gfx/Vita/p_world_render.cpp` (`materiau_env_prepare`).

### 6. Dynamic Memory Compaction Tuning
* **Current Status:** R36S has 1GB of physical RAM, which is plenty for THUG (originally targeted 64MB on Xbox).
* **Goal:** Audit memory pools in `Code/Sys/Mem/` to reduce unnecessary fragmentation without needing active swap file writes on slow micro-SD cards.

---

## 🌐 Low Priority / Future Research

### 7. Custom Soundtrack Support
* **Idea:** Allow players to place `.mp3` or `.ogg` files in `ports/thug/soundtrack/` that automatically get injected into the game's in-game radio playlist.

### 8. Local Ad-Hoc Multiplayer (Wi-Fi)
* **Idea:** Re-implement the Xbox system-link / LAN socket layer using standard Linux POSIX sockets (`sys/socket.h`), enabling two R36S consoles with OTG Wi-Fi to play HORSE, Graffiti, and Trick Attack.

