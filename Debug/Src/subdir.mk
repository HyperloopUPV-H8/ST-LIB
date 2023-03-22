################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/syscalls.c \
../Src/sysmem.c 

CPP_SRCS += \
../Src/ST-LIB.cpp 

OBJS += \
./Src/ST-LIB.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/syscalls.d \
./Src/sysmem.d 

CPP_DEPS += \
./Src/ST-LIB.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I"/home/alejandro/projects/ST-LIB/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/home/alejandro/projects/ST-LIB/Drivers/STM32H7xx_HAL_Driver/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_LOW" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Include" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Models" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Services" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/system" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/netif/ppp" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/netif" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/net" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/src/include/compat/stdc" -I"/home/alejandro/projects/ST-LIB/Middlewares/Third_Party/LwIP/system/arch" -I"/home/alejandro/projects/ST-LIB/Drivers/BSP/Components" -I"/home/alejandro/projects/ST-LIB/LWIP/App" -I"/home/alejandro/projects/ST-LIB/LWIP/Target" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_HIGH" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Inc -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_LOW" -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Models" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Include" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Services" -I"/home/alejandro/projects/ST-LIB/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/BSP/Components" -I"/home/alejandro/projects/ST-LIB/LWIP/App" -I"/home/alejandro/projects/ST-LIB/LWIP/Target" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_HIGH" -O0 -ffunction-sections -fdata-sections -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/ST-LIB.cyclo ./Src/ST-LIB.d ./Src/ST-LIB.o ./Src/ST-LIB.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

