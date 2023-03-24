#!/bin/bash

rm CMakeCache.txt
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
make -j16 all
