#pragma once

#include "HALAL/HALAL.hpp" // Erase this when ST-LIB_LOW is finished.
#include "ST-LIB_LOW.hpp"  // Erase this when ST-LIB_HIGH is finished
#include "ST-LIB_HIGH.hpp" // Highest layer up to the moment

class STLIB {
public:
	static void start(
			TARGET target = Board,
			string ip = "192.168.21.0",
			string subnet_mask = "",
			string gateway = "",
			UART::Peripheral& printf_peripheral = UART::uart2);
};
