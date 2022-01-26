#!/usr/bin/env bash
# Copyright (c) the JPEG XL Project Authors. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
set -e

DIR=$(realpath "$(dirname "$0")")
mkdir -p "$DIR"/build
cd "$DIR"/build

# set CXX to clang++ if not set in the environment
CXX="${CXX-clang++}"
if ! command -v "$CXX" >/dev/null ; then
  printf >&2 '%s: C++ compiler not found\n' "${0##*/}"
  exit 1
fi

[ -f lodepng.cpp ] || curl -o lodepng.cpp --url 'https://raw.githubusercontent.com/lvandeve/lodepng/8c6a9e30576f07bf470ad6f09458a2dcd7a6a84a/lodepng.cpp'
[ -f lodepng.h ] || curl -o lodepng.h --url 'https://raw.githubusercontent.com/lvandeve/lodepng/8c6a9e30576f07bf470ad6f09458a2dcd7a6a84a/lodepng.h'
[ -f lodepng.o ] || "$CXX" lodepng.cpp -O3 -mavx2 -o lodepng.o -c

"$CXX" -O3 -mavx2 -DFASTLL_ENABLE_AVX2_INTRINSICS -fopenmp \
  -I. lodepng.o \
  "$DIR"/fast_lossless.cc "$DIR"/fast_lossless_main.cc \
  -o fast_lossless