#!/bin/bash -e

mkdir -p build
cd build

source ../local-build.env

if [[ ! -f Makefile ]]; then
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make scratch
fi

make $@
