cmake_minimum_required (VERSION 3.4)
project (ST-LIB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY st-lib)

add_library(${LIBRARY} STATIC Src/ST-LIB.cpp)

target_compile_definitions(${LIBRARY} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDEBUG
)

target_compile_options(${LIBRARY} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  
  -O0
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  -fno-rtti
  -fno-use-cxa-atexit

  -Wall

  --specs=nano.specs
)

target_link_options(${LIBRARY} PRIVATE
        -T${CMAKE_SOURCE_DIR}/BSP/STM32F413ZHTx_FLASH.ld
        -mcpu=cortex-m7
        -mthumb
        -mfpu=fpv5-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
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
  COMMAND ls -lh libst-lib.a
  COMMAND echo ""
  )

