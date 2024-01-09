#pragma once

#include "HALconfig/HALconfig.hpp"
#include "DMA/DMA.hpp"
#include "DigitalOutputService/DigitalOutputService.hpp"
#include "DigitalInputService/DigitalInputService.hpp"
#include "Flash/Flash.hpp"
#include "Flash/FlashTests/Flash_Test.hpp"
#include "ADC/ADC.hpp"
#include "PWM/PWM/PWM.hpp"
#include "PWM/DualPWM/DualPWM.hpp"
#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPhasedPWM/DualPhasedPWM.hpp"
#include "Time/Time.hpp"
#include "InputCapture/InputCapture.hpp"
#include "Encoder/Encoder.hpp"
#include "EXTI/EXTI.hpp"
#include "Communication/SPI/SPI.hpp"
#include "Communication/UART/UART.hpp"
#include "Communication/I2C/I2C.hpp"
#include "Communication/Ethernet/UDP/DatagramSocket.hpp"
#include "Communication/Ethernet/TCP/ServerSocket.hpp"
#include "Communication/Ethernet/TCP/Socket.hpp"
#include "Communication/Ethernet/Ethernet.hpp"
#include "Communication/FDCAN/FDCAN.hpp"
#include "Communication/SNTP/SNTP.hpp"
#include "CORDIC/CORDIC.hpp"
#include "FMAC/FMAC.hpp"

namespace HALAL {
	void start(IPV4 ip, IPV4 subnet_mask, IPV4 gateway, UART::Peripheral& printf_peripheral);
}
