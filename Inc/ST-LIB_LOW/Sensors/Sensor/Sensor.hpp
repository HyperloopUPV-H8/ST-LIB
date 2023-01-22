/*
 * Sensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


template <typename T>

//asi es como deberia funcionar con template y virtual
class Sensor{
public:
	virtual void read() = 0;

protected:
	Pin pin;
	uint8_t id;
	T *value;
};

class SensorStarter{
public:
	static void start();
	static vector<uint8_t> adc_id_list;
	static vector<uint8_t> EXTI_id_list;
};

