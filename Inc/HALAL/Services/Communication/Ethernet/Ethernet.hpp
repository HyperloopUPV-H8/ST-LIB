/*
 * EthernetNode.cpp
 *
 *  Created on: Nov 23, 2022
 *      Author: stefa
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "PinModel/Pin.hpp"
#include "EthernetNode.hpp"
#include "lwip.h"
#include "ethernetif.h"

#ifdef HAL_ETH_MODULE_ENABLED

#define ETHERNET_POOLS_BASE_ADDRESS 0x30000000

class Ethernet{
public:
	static bool is_ready;
	static bool is_running;

	static void inscribe();
	static void start(IPV4 local_ip, IPV4 subnet_mask, IPV4 gateway);
	static void update();

private:
	static void mpu_start();
};

#endif
