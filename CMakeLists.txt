cmake_minimum_required (VERSION 3.4)

include(arm-none-eabi.cmake)


project (ST-LIB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY st-lib)

source_group(DRIVERS_C 
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/lan8742/lan8742.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_adc.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_adc_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_cordic.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_cortex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_dma.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_dma_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_eth.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_eth_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_exti.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_fdcan.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_flash.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_flash_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_fmac.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_gpio.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_hsem.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_i2c_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_lptim.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_mdma.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_pwr.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_pwr_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_rcc.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_rcc_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_rtc.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_rtc_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_spi_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_tim.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_tim_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_uart.c
  ${CMAKE_CURRENT_SOURCE_DIR}Drivers/STM32H7xx_HAL_Driver/smt32h7xx_hal_uart_ex.c
  )
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/ *.cpp)
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/ *.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/ *.hpp)

add_library(${LIBRARY} STATIC
  ${SOURCE_H}
  ${DRIVERS_C}
  ${SOURCE_HPP}
  ${SOURCE_CPP}
)

target_compile_definitions(${LIBRARY} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDEBUG
)

option(BOARD "Board clock config" OFF)
option(NUCLEO "Nucleo clock config" OFF)
set(NUCLEO 1)
if(BOARD)
  add_definitions(-DBOARD)
  add_definitions(-DHSE_VALUE=25000000)
endif()

if(NUCLEO)
  add_definitions(-DNUCLEO)
  add_definitions(-DHSE_VALUE=8000000)
endif()


target_compile_options(${LIBRARY} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  -Og
  -g3
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>

  -Wno-psabi
  -specs=/usr/lib/arm-none-eabi/lib/nosys.specs
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
set(CMAKE_CROSSCOMPILING TRUE)
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

