cmake_minimum_required(VERSION 3.16)
project(thug_r36s CXX C ASM)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_ASM_COMPILER aarch64-linux-gnu-gcc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++98 -fpermissive -fno-exceptions -fno-rtti -fno-strict-aliasing -march=armv8-a+crc -mtune=cortex-a35 -O3 -fno-math-errno -fno-trapping-math -fomit-frame-pointer -ffunction-sections -fdata-sections -Wno-narrowing -Wno-write-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -march=armv8-a+crc -mtune=cortex-a35 -O3 -fno-math-errno -fno-trapping-math -fomit-frame-pointer -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -Wl,--gc-sections")

add_compile_definitions(
  __PLAT_WN32__
  __PLAT_VITA__
  __PLAT_LINUX__
  KISAK_ORIGINAL_ALLOCATOR
  THUG_RELEASE
  THUG_SANS_MENU_VITA
  _GNU_SOURCE
  THUG_FMV
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/linux/ffmpeg-bink/include
  ${CMAKE_CURRENT_SOURCE_DIR}/linux/include
  ${CMAKE_CURRENT_SOURCE_DIR}/linux
  ${CMAKE_CURRENT_SOURCE_DIR}/Code
  ${CMAKE_CURRENT_SOURCE_DIR}/Code/Sk
  /usr/include/SDL2
  /usr/include/aarch64-linux-gnu
)

set(THUG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
include("${CMAKE_CURRENT_SOURCE_DIR}/vita/engine.cmake")

set(LINUX_SOURCES
  linux/platform_linux.cpp
  linux/audioout.cpp
  Code/Gel/Movies/Vita/vita_bink.c
)

add_executable(thug.aarch64 ${LINUX_SOURCES} ${THUG_SOURCES})

target_link_libraries(thug.aarch64
  -L/usr/lib/aarch64-linux-gnu
  SDL2
  GL
  GLESv2
  EGL
  asound
  ${CMAKE_CURRENT_SOURCE_DIR}/linux/ffmpeg-bink/lib/libavformat.a
  ${CMAKE_CURRENT_SOURCE_DIR}/linux/ffmpeg-bink/lib/libavcodec.a
  ${CMAKE_CURRENT_SOURCE_DIR}/linux/ffmpeg-bink/lib/libavutil.a
  pthread
  dl
  m
  z
)
