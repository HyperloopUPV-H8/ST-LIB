################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Src/ST-LIB_LOW/StateMachine/subdir.mk
-include Src/ST-LIB_LOW/Sensors/SensorInterrupt/subdir.mk
-include Src/ST-LIB_LOW/Sensors/Sensor/subdir.mk
-include Src/ST-LIB_LOW/Sensors/LookupSensor/subdir.mk
-include Src/ST-LIB_LOW/Sensors/LinearSensor/subdir.mk
-include Src/ST-LIB_LOW/Sensors/EncoderSensor/subdir.mk
-include Src/ST-LIB_LOW/Sensors/DigitalSensor/subdir.mk
-include Src/ST-LIB_LOW/Math/subdir.mk
-include Src/ST-LIB_LOW/ErrorHandler/subdir.mk
-include Src/ST-LIB_LOW/Clocks/subdir.mk
-include Src/ST-LIB_LOW/Actuator/PWM/subdir.mk
-include Src/ST-LIB_LOW/Actuator/HalfBridge/subdir.mk
-include Src/ST-LIB_LOW/Actuator/DigitalOutput/subdir.mk
-include Src/ST-LIB_LOW/subdir.mk
-include Src/ST-LIB_HIGH/Protections/subdir.mk
-include Src/HALAL/Services/Time/subdir.mk
-include Src/HALAL/Services/PWMService/subdir.mk
-include Src/HALAL/Services/InputCapture/subdir.mk
-include Src/HALAL/Services/Flash/FlashTests/subdir.mk
-include Src/HALAL/Services/Flash/subdir.mk
-include Src/HALAL/Services/Encoder/subdir.mk
-include Src/HALAL/Services/EXTI/subdir.mk
-include Src/HALAL/Services/DigitalOutputService/subdir.mk
-include Src/HALAL/Services/DigitalInputService/subdir.mk
-include Src/HALAL/Services/Communication/UART/subdir.mk
-include Src/HALAL/Services/Communication/SPI/subdir.mk
-include Src/HALAL/Services/Communication/SNTP/subdir.mk
-include Src/HALAL/Services/Communication/FDCAN/subdir.mk
-include Src/HALAL/Services/Communication/Ethernet/UDP/subdir.mk
-include Src/HALAL/Services/Communication/Ethernet/TCP/subdir.mk
-include Src/HALAL/Services/Communication/Ethernet/subdir.mk
-include Src/HALAL/Services/CORDIC/subdir.mk
-include Src/HALAL/Services/ADC/subdir.mk
-include Src/HALAL/Models/TimerPeripheral/subdir.mk
-include Src/HALAL/Models/PinModel/subdir.mk
-include Src/HALAL/Models/Packets/subdir.mk
-include Src/HALAL/Models/LowPowerTimer/subdir.mk
-include Src/HALAL/Models/IPV4/subdir.mk
-include Src/HALAL/Models/HALconfig/subdir.mk
-include Src/HALAL/Models/DMA/subdir.mk
-include Src/HALAL/subdir.mk
-include Src/subdir.mk
-include Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/subdir.mk
-include Drivers/BSP/Components/lan8742/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ST-LIB
BUILD_ARTIFACT_EXTENSION := a
BUILD_ARTIFACT_PREFIX := lib
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
ARCHIVES += \
libST-LIB.a \


# All Target
all: main-build

# Main-build Target
main-build: libST-LIB.a secondary-outputs

# Tool invocations
libST-LIB.a: $(OBJS) $(USER_OBJS) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-ar -r  "libST-LIB.a" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) libST-LIB.a
	-@echo ' '

secondary-outputs:

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
