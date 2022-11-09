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
	virtual void read();

protected:
	Pin pin;
	uint8_t id;
	T *value;
};
