# THUG-R36S: Tony Hawk's Underground 1 (PortMaster / Linux ARM64)

[![Status](https://img.shields.io/badge/Status-Work_In_Progress_(WIP)-orange.svg)](#status)
[![Platform](https://img.shields.io/badge/Platform-Linux_ARM64_|_KMSDRM-blue.svg)](#supported-hardware)
[![Target](https://img.shields.io/badge/Target-Rockchip_RK3326_(R36S_|_RG351_|_RGB20S)-green.svg)](#supported-hardware)
[![License](https://img.shields.io/badge/License-GPL_v2_/_MIT_Shim-yellow.svg)](#license)

An open-source, clean-room native ARM64 port of **Tony Hawk's Underground (THUG 1)** tailored for low-power handheld devices powered by the **Rockchip RK3326** SoC (such as the **R36S**, Anbernic RG351 series, Powkiddy RGB20S) running **ArkOS**, **AmberELEC**, or **ROCKNIX** via PortMaster and KMSDRM.

---

> [!IMPORTANT]
> **LEGAL NOTICE & CLEAN ROOM POLICY:**
> This repository contains **NO copyrighted game assets, proprietary code, audio tracks, FMVs, or level geometry**.
> To play the game, you must legally own a copy of the original **Tony Hawk's Underground (Xbox version)** and provide the extracted `Data/` directory.

---

## 🎮 Status & Feature Matrix

The port is currently in an active **Work In Progress (WIP)** stage. It is fully playable from start to finish across major modes, but open to community enhancements.

| Feature / Game Mode | Status | Details |
| :--- | :---: | :--- |
| **Story Mode (Modo História)** | ✅ **Working** | Full mission progression (New Jersey, Manhattan, Tampa, etc.), chapter cutscenes, dialogue, and goals. |
| **High Score / Free Skate** | ✅ **Working** | Timed sessions, trick combos, gap tracking, high score table persistence. |
| **Single Session** | ✅ **Working** | Standard 2-minute competitive skate sessions. |
| **Create-A-Skater (CAS)** | ✅ **Working** | Custom face, hair, clothing, logos, and deck graphics with corrected alpha testing and texture swapping. |
| **Create-A-Park (Park Editor)**| ✅ **Working** | Place ramps, rails, and gaps using the level sector cloning system. |
| **In-Engine Cutscenes** | ✅ **Working** | Camera movement, skater animations, and vblank synchronization fully operational. |
| **FMV / Pre-rendered Movies** | ✅ **Working** | Bink Video (.bik) decoding with synchronized stereo audio via integrated FFmpeg. |
| **Audio (BGM & SFX)** | ✅ **Working** | Full original soundtrack streaming and sound effects via ALSA and OpenAL. |
| **Gamepad Controls** | ✅ **Working** | Native SDL2 GameController polling + `gptokeyb` mappings for R36S dual-analog & button layout. |
| **Graphics & Optimization** | ✅ **Working** | 60 FPS target on RK3326, GL4ES GLES 2.0 pipeline, 16-bit texture format, punch-through alpha fixes. |
| **Create-A-Goal / OSK** | ⚠️ **WIP** | Text entry menus functional but benefit from physical USB keyboard or on-screen virtual keyboard improvements. |
| **Multiplayer / LAN** | ❌ **Disabled** | Xbox Live and LAN code stubbed for local single-player portability. |
| **Dynamic Shadow Maps** | 🔄 **Simplified** | Hardware shadow maps replaced with optimized simple skater shadow to save GPU fillrate. |
| **Parametric Particles** | 🔄 **Optimized** | High-overhead CPU particle loops disabled to ensure smooth 60 FPS gameplay on in-order Cortex-A35 cores. |

---

## 🛠️ Hardware Requirements & Performance Profile

* **SoC:** Rockchip RK3326 (Quad-core ARM Cortex-A35 @ 1.3GHz)
* **GPU:** ARM Mali-G31 MP2 (supports OpenGL ES 2.0 / EGL)
* **RAM:** 1 GB LPDDR3 (recommended: 256MB+ zram or swap enabled)
* **Display Output:** 640x480 @ 60Hz via KMSDRM
* **Tested Distributions:** ArkOS (recommended), AmberELEC, ROCKNIX

---

## 📂 Installation & Setup (For Players)

1. **Download the PortMaster Package:**
   Place `THUG.sh` into your SD card under `/roms/ports/` (or `/roms2/ports/`).
   Place the `thug/` folder into `/roms/ports/thug/`.

2. **Supply Original Game Assets:**
   Obtain a legally owned disc image of **Tony Hawk's Underground (Xbox)** and extract its contents using an Xbox ISO extraction utility (e.g., `extract-xiso`).
   Copy the entire `Data` folder from the root of the Xbox game into:
   ```text
   ports/thug/Data/
   ```
   *(Case-sensitive filesystems like ext4 are handled automatically by the launch script, but verify naming matches `Data`).*

3. **Launch the Game:**
   Start **Tony Hawk's Underground** from the **Ports** menu on your device.

---

## 🏗️ Compiling from Source

We recommend using Docker for deterministic, clean cross-compilation targeting `aarch64-linux-gnu`.

### 1. Build using Docker
```bash
# From the repository root:
docker run --rm -v "$(pwd):/workspace" -w /workspace/build_linux r36s-builder cmake --build . --config Release -j4
```

The resulting binary will be output to:
```text
build_linux/thug.aarch64
```

---

## 🤝 Contributing, Forks & PRs

Contributions are warmly welcomed! Whether you want to:
* Re-implement lightweight GPU shaders for grind sparks and particles
* Add an on-screen keyboard (OSK) for text input in Create-A-Goal
* Tune LOD distances or optimize asset streaming
* Package metadata for the official PortMaster catalog submission

Please check [CONTRIBUTING.md](CONTRIBUTING.md) and review open items in [BACKLOG.md](BACKLOG.md).
Feel free to open an issue, submit a pull request, or fork the repository!

---

## 📜 Changelog

See [CHANGELOG.md](CHANGELOG.md) for full release notes and milestone breakdown.

---

## 🙏 Credits & Acknowledgments

This port stands on the shoulders of giants. Immense gratitude goes to:

* **Neversoft Entertainment & Activision:** For creating Tony Hawk's Underground, one of the greatest skateboarding games of all time.
* **iwannagooutside ([THUG-Vita](https://github.com/iwannagooutside/thug-vita-port)):** For the reverse-engineering foundation, Xbox asset parsers, and initial porting effort.
* **Rinnegatamante:** For **vitaGL** and foundational graphics translation patterns.
* **ptitSeb ([GL4ES](https://github.com/ptitSeb/gl4es)):** For the outstanding OpenGL 2.1 to GLES 2.0 translation wrapper, enabling complex legacy pipelines to run on Mali-G31.
* **The PortMaster Team (christianhaitian, kloptops, cebion):** For the standard porting framework, runtime libraries, and gamepad mappings (`gptokeyb`).
* **The FFmpeg Project:** For robust Bink audio/video decoding.
* **ArkOS / Rockchip Linux Developers:** For kernel DRM/KMS drivers and performance governors.

---

## ⚖️ License

The source code modifications, Linux platform wrappers, and build configurations are distributed under the **MIT License** / **GPL v2** where applicable. All game assets, trademarks, and intellectual property belong to Activision Publishing, Inc.
