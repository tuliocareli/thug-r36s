# Contributing to THUG-R36S

Thank you for your interest in contributing to **THUG-R36S**!
This project is open-source, community-driven, and thrives on collaboration, testing, and feedback.

---

## ⚖️ Clean-Room Policy (STRICT)

> [!CAUTION]
> **NEVER COMMIT OR SUBMIT ANY COPYRIGHTED ASSETS!**
> * No `.xbx`, `.pre`, `.bik`, `.img`, `.wav`, `.pcm`, or `.iso` files.
> * No decompiled binary dumps or proprietary Activision/Neversoft proprietary binaries.
> * Any Pull Request containing copyrighted game assets will be closed and rejected immediately.

---

## 🛠️ Getting Started & Build Setup

1. **Fork the Repository:**
   Click the **Fork** button on GitHub to create your own copy.

2. **Clone your Fork:**
   ```bash
   git clone https://github.com/<your-username>/thug-r36s.git
   cd thug-r36s
   ```

3. **Compile using Docker:**
   Ensure you have Docker installed and the `r36s-builder` container ready:
   ```bash
   docker run --rm -v "$(pwd):/workspace" -w /workspace/build_linux r36s-builder cmake --build . --config Release -j4
   ```

4. **Testing on Hardware:**
   Copy `build_linux/thug.aarch64` to your SD card under `ports/thug/thug.aarch64` and test on your R36S, RG351, or compatible RK3326 device.

---

## 📐 Coding Principles & Guidelines

We follow **Clean Code** standards to keep this complex codebase manageable:
* **Predictable & Simple:** Avoid overengineering and unnecessary abstractions. Prefer solutions that are easy to maintain and understand.
* **Readable Over Clever:** A descriptive function or variable name is far better than short, cryptic abbreviations.
* **Document Assumptions:** If you implement a workaround for an OpenGL or hardware quirk, explain *why* in a comment above the code.
* **Preserve Integrity:** Maintain existing comments and docstrings unrelated to your changes.
* **Platform Guards:** Always guard Linux-specific adaptations with `#if defined(__PLAT_LINUX__)` to prevent breaking other potential targets.

---

## 🔄 Pull Request Workflow

1. Create a feature branch:
   ```bash
   git checkout -b feature/my-cool-optimization
   ```
2. Commit your changes with clear, descriptive commit messages:
   ```bash
   git commit -m "gfx: add GLES 2.0 particle billboard vertex shader"
   ```
3. Push to your fork:
   ```bash
   git push origin feature/my-cool-optimization
   ```
4. Open a **Pull Request** targeting the `main` branch.
5. In your PR description, describe:
   * What problem this fixes or feature it introduces.
   * Testing done (which device, distribution, and framerate observed).
   * Any visual trade-offs or performance metrics.

---

## 🐛 Reporting Bugs

When opening an Issue, please include:
1. **Device Model:** (e.g. R36S V5, RG351MP, RGB20S)
2. **Distribution & Version:** (e.g. ArkOS 04242024, AmberELEC 20240315)
3. **Exact Steps to Reproduce:** Level, mission, or action performed.
4. **Log Output:** Attach `ports/thug/log.txt` and `ports/thug/launcher.log`.
