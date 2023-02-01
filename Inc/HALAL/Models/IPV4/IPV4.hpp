#pragma once

#include "stm32h7xx_hal.h"

#ifdef HAL_ETH_MODULE_ENABLED

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "netif.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "timeouts.h"
#include "C++Utilities/CppUtils.hpp"

using std::stringstream;
using std::getline;

class IPV4{
public:
	ip_addr_t address;
	string string_address;

	IPV4();
	IPV4(string address);

};
#endif
