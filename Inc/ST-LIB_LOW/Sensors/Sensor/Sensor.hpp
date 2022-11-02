/*
 * Sensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


class Sensor{
public:
	Sensor(value_type type);
	virtual void read(auto *value);

private:
	value_type type;

};
