#pragma once
#include "stm32h7xx_hal.h"
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


class IPV4{
public:
	ip_addr_t ip_address;
	string string_ip;

	IPV4();

	IPV4(string ip_address);

};