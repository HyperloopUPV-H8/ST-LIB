cmake_minimum_required (VERSION 3.4)
project (ST-LIB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(SOURCES ST-LIB.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(st-lib PUBLIC
  Drivers/CMSIS/Device/ST/STM32H7xx/Include
  Drivers/CMSIS/Include
  Drivers/STM32H7xx_HAL_Driver/Inc
  Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
  Inc
  Inc/HALAL/Models
  Inc/HALAL/Services
  Inc/ST-LIB_LOW
  Inc/ST-LIB_HIGH
)
add_library(st-lib STATIC Src/ST-LIB.cpp)
