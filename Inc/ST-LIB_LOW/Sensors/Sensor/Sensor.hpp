/*
 * Sensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"



//asi es como deberia funcionar con template y virtual
class Sensor{
public:
	static void start();
	static vector<uint8_t> adc_id_list;
	static vector<uint8_t> EXTI_id_list;
};

