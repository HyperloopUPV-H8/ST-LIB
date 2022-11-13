#pragma once

#include "ST-LIB.hpp"

class Ethernet{

	IPV4 local_ip;

	uint32_t local_port;

	static void register_eth(IPV4 local_ip, uint32_t local_port);

	static void update();

};
