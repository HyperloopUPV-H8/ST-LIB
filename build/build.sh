#!/bin/bash

echo "Enter Target (nucleo|n / board|b) : "
read TARGET

cmake --build . --target clean

if ([ $TARGET = "board" ] || [ $TARGET = "b" ] || [ $TARGET = "B" ] || [ $TARGET = "Board" ] || [ $TARGET = "BOARD" ])
then
  TARGET="BOARD"
  echo "board!"
else
  if ([ $TARGET = "nucleo" ] || [ $TARGET = "n" ] || [ $TARGET = "N" ] || [ $TARGET = "Nucleo" ]  || [ $TARGET = "NUCLEO" ])
  then
    TARGET="NUCLEO"
    echo "nucleo!"
  else
    echo "Wrong value, exiting"
    exit 1
  fi
fi

cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake -D${TARGET}=ON ..
make -j16 all

rm -R CMakeCache.txt CMakeFiles cmake_install.cmake CMakeLists.txt Makefile 2> /dev/null

mv compile_commands.json ..
