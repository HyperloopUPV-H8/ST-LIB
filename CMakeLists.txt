cmake_minimum_required (VERSION 3.4)
project (ST-LIB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY st-lib)

file(GLOB_RECURSE SOURCE_C ${CMAKE_SOURCE_DIR}/ *.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/ *.cpp)
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/ *.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/ *.hpp)

add_library(${LIBRARY} STATIC
  ${SOURCE_C}
  ${SOURCE_CPP}
  ${SOURCE_H}
  ${SOURCE_CPP}
)

target_compile_definitions(${LIBRARY} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDEBUG
)

option(BOARD "Board clock config" OFF)
option(NUCLEO "Nucleo clock config" OFF)

if(BOARD)
  add_definitions(-DNUCLEO)
  add_definitions(-DHSE_VALUE=25000000)
endif()

if(NUCLEO)
  add_definitions(-DBOARD)
  add_definitions(-DHSE_VALUE=8000000)
endif()


target_compile_options(${LIBRARY} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  
  -O0
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>

  -Wno-psabi
  --specs=nosys.specs
)

target_include_directories(${LIBRARY} PUBLIC
  Drivers/CMSIS/Device/ST/STM32H7xx/Include
  Drivers/CMSIS/Include
  Drivers/STM32H7xx_HAL_Driver/Inc
  Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
  Middlewares/Third_Party/LwIP/src/include
  Middlewares/Third_Party/LwIP/system
  Middlewares/Third_Party/LwIP/src/netif/ppp
  Middlewares/Third_Party/LwIP/src/include/lwip
  Middlewares/Third_Party/LwIP/src/include/lwip/apps
  Middlewares/Third_Party/LwIP/src/include/lwip/priv
  Middlewares/Third_Party/LwIP/src/include/lwip/prot
  Middlewares/Third_Party/LwIP/src/include/netif
  Middlewares/Third_Party/LwIP/src/include/compat
  Middlewares/Third_Party/LwIP/src/include/compat/posix
  Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa
  Middlewares/Third_Party/LwIP/src/include/compat/posix/net
  Middlewares/Third_Party/LwIP/src/include/compat/posix/sys
  Middlewares/Third_Party/LwIP/src/include/compat/stdc
  Middlewares/Third_Party/LwIP/system/arch
  Drivers/BSP/Components
  Drivers/BSP/Components/lan8742
  LWIP/App
  LWIP/Target
  Inc
  Inc/HALAL/Models
  Inc/HALAL/Services
  Inc/ST-LIB_LOW
  Inc/ST-LIB_HIGH
)

IF(CMAKE_CROSSCOMPILING)
  message(STATUS "Cross-compiling")
ELSE()
  message(STATUS "NOT Cross-compiling")
ENDIF()

add_custom_command(TARGET ${LIBRARY}
  POST_BUILD
  COMMAND echo ""
  COMMAND echo ""
  COMMAND echo ""
  COMMAND ls -lh libst-lib.a
  COMMAND echo ""
  COMMAND echo ""
  COMMAND echo ""
  )

