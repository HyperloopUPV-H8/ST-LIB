/*
 * LookupSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


class LookupSensor : public AnalogSensor{
public:
	LookupSensor(Pin pin, double *table);
	void read(double &value);
	uint8_t getID();

private:
	uint8_t id;
	Pin pin;

};
