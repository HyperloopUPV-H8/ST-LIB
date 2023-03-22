################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/ST-LIB_LOW/Clocks/Counter.cpp \
../Src/ST-LIB_LOW/Clocks/Stopwatch.cpp 

OBJS += \
./Src/ST-LIB_LOW/Clocks/Counter.o \
./Src/ST-LIB_LOW/Clocks/Stopwatch.o 

CPP_DEPS += \
./Src/ST-LIB_LOW/Clocks/Counter.d \
./Src/ST-LIB_LOW/Clocks/Stopwatch.d 


# Each subdirectory must supply rules for building sources it contributes
Src/ST-LIB_LOW/Clocks/%.o Src/ST-LIB_LOW/Clocks/%.su Src/ST-LIB_LOW/Clocks/%.cyclo: ../Src/ST-LIB_LOW/Clocks/%.cpp Src/ST-LIB_LOW/Clocks/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I"/home/alejandro/projects/ST-LIB/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/home/alejandro/projects/ST-LIB/Drivers/STM32H7xx_HAL_Driver/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_LOW" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Include" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Models" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Services" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/system" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/netif/ppp" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/netif" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/net" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/stdc" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/system/arch" -I"/home/alejandro/projects/ST-LIB/Drivers/BSP/Components" -I"/home/alejandro/projects/ST-LIB/LWIP/App" -I"/home/alejandro/projects/ST-LIB/LWIP/Target" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_HIGH" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-ST-2d-LIB_LOW-2f-Clocks

clean-Src-2f-ST-2d-LIB_LOW-2f-Clocks:
	-$(RM) ./Src/ST-LIB_LOW/Clocks/Counter.cyclo ./Src/ST-LIB_LOW/Clocks/Counter.d ./Src/ST-LIB_LOW/Clocks/Counter.o ./Src/ST-LIB_LOW/Clocks/Counter.su ./Src/ST-LIB_LOW/Clocks/Stopwatch.cyclo ./Src/ST-LIB_LOW/Clocks/Stopwatch.d ./Src/ST-LIB_LOW/Clocks/Stopwatch.o ./Src/ST-LIB_LOW/Clocks/Stopwatch.su

.PHONY: clean-Src-2f-ST-2d-LIB_LOW-2f-Clocks

