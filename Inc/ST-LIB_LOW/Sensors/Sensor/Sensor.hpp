/*
 * Sensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "C++Utilities/CppUtils.hpp"
#include "ADC/ADC.hpp"
#include "EXTI/EXTI.hpp"

class Sensor{
public:
	static void start();
	static vector<uint8_t> adc_id_list;
	static vector<uint8_t> EXTI_id_list;
};

