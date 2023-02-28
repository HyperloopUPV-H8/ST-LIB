/*
 * SNTP.hpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

//https://community.st.com/s/question/0D53W00000j5I4aSAE/how-set-up-pretty-presice-and-detailed-rtc-on-stm32h743zi2

//tontos usando sntp para configurar rtc

#pragma once

#include <lwip/ip_addr.h>
#include "sntp.h"

class SNTP{
public:
	static void sntp_update();
	static const ip4_addr *sntp_server;

};
