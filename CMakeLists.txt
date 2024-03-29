cmake_minimum_required (VERSION 3.4)

include(arm-none-eabi.cmake)

project (ST-LIB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY st-lib)
  
file(GLOB_RECURSE SOURCE_C ${CMAKE_SOURCE_DIR}/ *.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/ *.cpp)
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/ *.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/ *.hpp)



if(${NUCLEO})
  add_definitions(-DNUCLEO)
  add_definitions(-DHSE_VALUE=8000000)
else()
  add_definitions(-DBOARD)
  add_definitions(-DHSE_VALUE=25000000)
endif()

if(${RELEASE})
  set(OPTIMIZATION -O3)
  set(DEBUG_CONFIGURATION -g0)
else()
  set(OPTIMIZATION -O0)
  set(DEBUG_CONFIGURATION -g3)
endif()


if(${ETHERNET})
  set(ETH STLIB_ETH)
endif()
message("\n\nCMAKE:\n")
message(STATUS "ETHERNET is ENABLED ?? " ${ETHERNET})

add_library(${LIBRARY} STATIC
  ${SOURCE_H}
  ${SOURCE_C}
  ${SOURCE_HPP}
  ${SOURCE_CPP}
)

target_compile_definitions(${LIBRARY} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDEBUG
  ${ETH}
  -DUSING_CMAKE
)




target_compile_options(${LIBRARY} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  ${OPTIMIZATION}
  ${DEBUG_CONFIGURATION}
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>

  -Wno-psabi
  -specs=nosys.specs
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

message(STATUS "[DEBUG, RELEASE]: RELEASE: " ${RELEASE})
message(STATUS "Optimization value: " ${OPTIMIZATION})
message(STATUS "Debug configuration value: " ${DEBUG_CONFIGURATION})
message(STATUS "[BOARD, NUCLEO] Config: NUCLEO: " ${NUCLEO})

