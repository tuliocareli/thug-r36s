#!/bin/sh
set -e
rm -rf /tmp/ffmpeg /workspace/linux/ffmpeg-bink
git clone --depth 1 --branch release/5.1 https://github.com/FFmpeg/FFmpeg.git /tmp/ffmpeg
cd /tmp/ffmpeg
./configure --prefix=/workspace/linux/ffmpeg-bink \
  --enable-cross-compile --cross-prefix=aarch64-linux-gnu- \
  --arch=aarch64 --target-os=linux \
  --disable-runtime-cpudetect \
  --disable-everything \
  --enable-decoder=bink,binkaudio_dct,binkaudio_rdft \
  --enable-demuxer=bink \
  --disable-programs --disable-doc --disable-network --disable-autodetect \
  --enable-static --disable-shared \
  --disable-avdevice --disable-swscale --disable-swresample --disable-avfilter \
  --disable-debug --enable-pthreads \
  --extra-cflags="-O2 -fPIC"
make -j$(nproc)
make install
echo "FFMPEG_BUILD_COMPLETE"
