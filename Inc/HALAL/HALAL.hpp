#pragma once

#include "HALconfig/HALconfig.hpp"
#include "DMA/DMA.hpp"
#include "DigitalOutputService/DigitalOutputService.hpp"
#include "DigitalInputService/DigitalInputService.hpp"
#include "Flash/Flash.hpp"
#include "Flash/FlashTests/Flash_Test.hpp"
#include "ADC/ADC.hpp"
#include "PWMService/PWMService.hpp"
#include "Time/Time.hpp"
#include "InputCapture/InputCapture.hpp"
#include "Encoder/Encoder.hpp"
#include "EXTI/EXTI.hpp"
#include "Communication/SPI/SPI.hpp"
#include "Communication/UART/UART.hpp"
#include "Communication/FDCAN/FDCAN.hpp"
#include "CORDIC/CORDIC.hpp"

namespace HALAL {
	void start();
}
