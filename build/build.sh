#!/bin/bash

cmake --build . --target clean
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
make -j16 all
