#pragma once

#include "HALAL/HALAL.hpp" // Erase this when ST-LIB_LOW is finished.
#include "ST-LIB_LOW.hpp"  // Erase this when ST-LIB_HIGH is finished
#include "ST-LIB_HIGH.hpp" // Highest layer up to the moment

class STLIB {
public:
	static void start(
			string ip = "192.168.1.4",
			string subnet_mask = "255.255.0.0",
			string gateway = "192.168.1.1",
			UART::Peripheral& printf_peripheral = UART::uart2);

	static void update();
};
