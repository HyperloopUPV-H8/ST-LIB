################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/HALAL/Services/DigitalInput/DigitalInput.cpp 

OBJS += \
./Src/HALAL/Services/DigitalInput/DigitalInput.o 

CPP_DEPS += \
./Src/HALAL/Services/DigitalInput/DigitalInput.d 


# Each subdirectory must supply rules for building sources it contributes
Src/HALAL/Services/DigitalInput/%.o Src/HALAL/Services/DigitalInput/%.su: ../Src/HALAL/Services/DigitalInput/%.cpp Src/HALAL/Services/DigitalInput/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++17 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-HALAL-2f-Services-2f-DigitalInput

clean-Src-2f-HALAL-2f-Services-2f-DigitalInput:
	-$(RM) ./Src/HALAL/Services/DigitalInput/DigitalInput.d ./Src/HALAL/Services/DigitalInput/DigitalInput.o ./Src/HALAL/Services/DigitalInput/DigitalInput.su

.PHONY: clean-Src-2f-HALAL-2f-Services-2f-DigitalInput

