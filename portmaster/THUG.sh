#!/bin/bash
# PortMaster Launch Script for Tony Hawk's Underground (THUG 1)
# Target Devices: R36S, RG351MP, RGB20S, RK2020 (Rockchip RK3326 ARM64)
# Supported OS: ArkOS, AmberELEC, ROCKNIX, TheRA

XDG_DATA_HOME=${XDG_DATA_HOME:-$HOME/.local/share}

if [ -d "/opt/system/Tools/PortMaster/" ]; then
  controlfolder="/opt/system/Tools/PortMaster"
elif [ -d "/opt/tools/PortMaster/" ]; then
  controlfolder="/opt/tools/PortMaster"
elif [ -d "$XDG_DATA_HOME/PortMaster/" ]; then
  controlfolder="$XDG_DATA_HOME/PortMaster"
else
  controlfolder="/roms/ports/PortMaster"
fi

if [ -f "$controlfolder/control.txt" ]; then
  source "$controlfolder/control.txt"
  [ -f "${controlfolder}/mod_${CFW_NAME}.txt" ] && source "${controlfolder}/mod_${CFW_NAME}.txt"
  get_controls
fi
ESUDO="${ESUDO:-sudo}"

CURR_TTY="/dev/tty1"
exec < "$CURR_TTY"

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
if [ -n "$directory" ] && [ -d "/$directory/ports/thug" ]; then
  GAMEDIR="/$directory/ports/thug"
else
  GAMEDIR="$SCRIPT_DIR/thug"
fi
cd "$GAMEDIR"

export DEVICE_ARCH="aarch64"
# NOTE: do NOT export LD_LIBRARY_PATH here. The bundled glibc in libs/ would be picked up by every
# system tool (sudo, tee, ls, gptokeyb) and break them. Only the game is started with
# --library-path (see the launch line below).
[ -n "$sdl_controllerconfig" ] && export SDL_GAMECONTROLLERCONFIG="$sdl_controllerconfig"

# R36S Mali-G31 ArkOS driver bindings & GL4ES environment
export SDL_VIDEODRIVER=kmsdrm
export SDL_AUDIODRIVER=alsa
export ALSOFT_DRIVERS=alsa
export AUDIODEV="default"

# Ensure ALSA sound channels are unmuted on ArkOS / PortMaster devices
amixer sset 'Playback' 100% unmute >/dev/null 2>&1 || true
amixer sset 'Master' 100% unmute >/dev/null 2>&1 || true
amixer sset 'Headphone' 100% unmute >/dev/null 2>&1 || true
amixer sset 'Speaker' 100% unmute >/dev/null 2>&1 || true
# R36S Mali-G31 ArkOS driver bindings & GL4ES environment
export SDL_VIDEO_GL_DRIVER="$GAMEDIR/libs/libGL.so.1"
export SDL_VIDEO_EGL_DRIVER=libEGL.so
export LIBGL_ES=2
export LIBGL_GL=21
export LIBGL_NOTEST=1
export LIBGL_NPOT=1
export LIBGL_VSYNC=0
export THUG_VSYNC=0
# export LIBGL_BATCH=4 (desativado para prevenir instabilidades/crashes em transicoes de menus)
export LIBGL_DXT=1
export LIBGL_FORCE16BITS=1
export LIBGL_FASTMATH=1
export LIBGL_NOERROR=1
export LIBGL_NOBANNER=1
export MALLOC_CHECK_=0

export THUG_WIDTH=640
export THUG_HEIGHT=480

# RK3326 CPU and GPU Performance governors
SUDO="${ESUDO:-sudo}"
CLOCK_STATE=""
for gov in /sys/devices/system/cpu/cpufreq/policy*/scaling_governor \
           /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor \
           /sys/class/devfreq/*/governor; do
  [ -f "$gov" ] || continue
  old=$(cat "$gov" 2>/dev/null)
  echo performance | $SUDO tee "$gov" > /dev/null 2>&1 && CLOCK_STATE="$CLOCK_STATE $gov=$old"
done
restore_clocks() {
  for entry in $CLOCK_STATE; do
    echo "${entry#*=}" | $SUDO tee "${entry%%=*}" > /dev/null 2>&1 || true
  done
}
trap restore_clocks EXIT

# Elevate process scheduling priority for game execution
$SUDO renice -n -10 $$ >/dev/null 2>&1 || true

# R36S 1GB RAM Protection & safety swap
sync
echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null 2>&1 || true

SWAP_TOTAL=$(free -m | awk '/Swap:/ {print $2}')
if [ -z "$SWAP_TOTAL" ] || [ "$SWAP_TOTAL" -lt 256 ]; then
  echo "[PortMaster] Ensuring 512MB safety swap on SD card..."
  if [ ! -f "$GAMEDIR/.swapfile" ]; then
    dd if=/dev/zero of="$GAMEDIR/.swapfile" bs=1M count=512 status=none
    mkswap "$GAMEDIR/.swapfile" >/dev/null 2>&1
  fi
  sudo swapon "$GAMEDIR/.swapfile" >/dev/null 2>&1
fi

# Verify game assets presence
if [ ! -d "$GAMEDIR/Data" ] && [ ! -d "$GAMEDIR/data" ]; then
  printf "\033c" > "$CURR_TTY"
  printf "======================================================================\n" > "$CURR_TTY"
  printf " ERROR: Game assets not found!\n" > "$CURR_TTY"
  printf " Please copy your Tony Hawk's Underground 'Data' folder into:\n" > "$CURR_TTY"
  printf "   %s/Data/\n" "$GAMEDIR" > "$CURR_TTY"
  printf "======================================================================\n" > "$CURR_TTY"
  sleep 5
  exit 1
fi

# Ensure both Data and data paths exist for case-sensitive filesystems (ext4)
if [ -d "$GAMEDIR/data" ] && [ ! -e "$GAMEDIR/Data" ]; then
  ln -sf "data" "$GAMEDIR/Data" 2>/dev/null || true
fi
if [ -d "$GAMEDIR/Data" ] && [ ! -e "$GAMEDIR/data" ]; then
  ln -sf "Data" "$GAMEDIR/data" 2>/dev/null || true
fi

# Unbind vtconsole to allow KMSDRM full access to DRM primary plane
printf "\033c" > "$CURR_TTY" 2>/dev/null || true
printf "\e[?25l" > "$CURR_TTY" 2>/dev/null || true
echo 0 | sudo tee /sys/class/vtconsole/vtcon0/bind > /dev/null 2>&1 || true
echo 0 | sudo tee /sys/class/vtconsole/vtcon1/bind > /dev/null 2>&1 || true

# Backup previous run log
[ -f "$GAMEDIR/log.txt" ] && mv -f "$GAMEDIR/log.txt" "$GAMEDIR/log.prev.txt"

# GPU / driver diagnostic report (launcher.log)
{
  echo "[launcher] uname: $(uname -a)"
  echo "[launcher] CFW: ${CFW_NAME:-unknown}"
  echo "[launcher] --- mali / EGL / GLES / gbm libraries on the system ---"
  ls -l /usr/lib/aarch64-linux-gnu /lib/aarch64-linux-gnu /usr/lib /usr/local/lib 2>/dev/null | grep -i -E "mali|libEGL|libGLES|libgbm|libdrm|libMali"
  find / -xdev \( -name "libmali*" -o -name "libMali*" \) 2>/dev/null
  echo "[launcher] --- /dev/mali* and /dev/dri ---"
  ls -l /dev/mali* /dev/dri 2>&1
  echo "[launcher] --- loader cache ---"
  ldconfig -p 2>/dev/null | grep -i -E "mali|libEGL|libGLES|libgbm"
  echo "[launcher] --- libs bundled in game dir that touch the GPU stack ---"
  ls -l "$GAMEDIR/libs" | grep -i -E "mali|EGL|GLES|gbm|drm|GLdispatch|libGL"
} > "$GAMEDIR/launcher.log" 2>&1

# Start Gamepad mapping via gptokeyb
if [ -n "${GPTOKEYB:-}" ]; then
  $GPTOKEYB "thug.aarch64" -c "./thug.gptk" &
fi

# Ensure executable permissions on binary and bundled dynamic linker
chmod +x ./thug.aarch64 "$GAMEDIR/libs/ld-linux-aarch64.so.1" 2>/dev/null || true

# Run Game via bundled dynamic linker (if present) or directly
echo "[launcher] Starting THUG execution: $(date)" > "$GAMEDIR/log.txt"
if [ -f "$GAMEDIR/libs/ld-linux-aarch64.so.1" ]; then
  # /usr/local/lib/aarch64-linux-gnu holds the Mali (gbm) driver matching the kernel module on ArkOS;
  # /usr/lib/aarch64-linux-gnu has an older wayland-gbm blob that /dev/mali0 rejects (user 10.6 vs kernel 11.38).
  "$GAMEDIR/libs/ld-linux-aarch64.so.1" --library-path "$GAMEDIR/libs:/usr/local/lib/aarch64-linux-gnu:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu" ./thug.aarch64 >> "$GAMEDIR/log.txt" 2>&1
  EXIT_CODE=$?
else
  ./thug.aarch64 >> "$GAMEDIR/log.txt" 2>&1
  EXIT_CODE=$?
fi
echo "[launcher] Game exited with code: $EXIT_CODE" >> "$GAMEDIR/log.txt"

# Restore console and background services on exit
if [ -n "${GPTOKEYB:-}" ]; then
  kill -9 $(pidof gptokeyb) 2>/dev/null || true
fi
echo 1 | sudo tee /sys/class/vtconsole/vtcon0/bind > /dev/null 2>&1 || true
echo 1 | sudo tee /sys/class/vtconsole/vtcon1/bind > /dev/null 2>&1 || true
$ESUDO systemctl restart oga_events >/dev/null 2>&1 &
printf "\033c" > "$CURR_TTY" 2>/dev/null || true
printf "\e[?25h" > "$CURR_TTY" 2>/dev/null || true

exit $EXIT_CODE
