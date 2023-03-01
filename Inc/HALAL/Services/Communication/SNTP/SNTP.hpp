/*
 * SNTP.hpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

//https://community.st.com/s/question/0D53W00000j5I4aSAE/how-set-up-pretty-presice-and-detailed-rtc-on-stm32h743zi2

//tontos usando sntp para configurar rtc

#pragma once

#include "C++Utilities/CppUtils.hpp"

class SNTP{
public:
	static void sntp_update(uint8_t address_head, uint8_t address_second, uint8_t address_third, uint8_t address_last);

};
