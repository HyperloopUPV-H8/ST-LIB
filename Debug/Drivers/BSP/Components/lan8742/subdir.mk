################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/lan8742/lan8742.c 

OBJS += \
./Drivers/BSP/Components/lan8742/lan8742.o 

C_DEPS += \
./Drivers/BSP/Components/lan8742/lan8742.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/lan8742/%.o Drivers/BSP/Components/lan8742/%.su Drivers/BSP/Components/lan8742/%.cyclo: ../Drivers/BSP/Components/lan8742/%.c Drivers/BSP/Components/lan8742/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Inc -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_LOW" -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Models" -I"/home/alejandro/projects/ST-LIB/Drivers/CMSIS/Include" -I"/home/alejandro/projects/ST-LIB/Inc/HALAL/Services" -I"/home/alejandro/projects/ST-LIB/Inc" -I"/home/alejandro/projects/ST-LIB/Drivers/BSP/Components" -I"/home/alejandro/projects/ST-LIB/LWIP/App" -I"/home/alejandro/projects/ST-LIB/LWIP/Target" -I"/home/alejandro/projects/ST-LIB/Inc/ST-LIB_HIGH" -O0 -ffunction-sections -fdata-sections -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-lan8742

clean-Drivers-2f-BSP-2f-Components-2f-lan8742:
	-$(RM) ./Drivers/BSP/Components/lan8742/lan8742.cyclo ./Drivers/BSP/Components/lan8742/lan8742.d ./Drivers/BSP/Components/lan8742/lan8742.o ./Drivers/BSP/Components/lan8742/lan8742.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-lan8742

