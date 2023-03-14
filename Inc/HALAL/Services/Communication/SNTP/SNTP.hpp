/*
 * SNTP.hpp
 *
 *  Created on: 21 feb. 2023
 *      Author: Ricardo
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"

class SNTP{
public:
	static void sntp_update(uint8_t address_head, uint8_t address_second, uint8_t address_third, uint8_t address_last);

};
