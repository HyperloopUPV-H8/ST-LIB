/*
 * LookupSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/AnalogSensor/AnalogSensor.hpp"

#define reference_voltage 3.3


class LookupSensor : public AnalogSensor::AnalogSensor{
public:
	LookupSensor(Pin pin, double *table, int table_size, double *value);
	void read();
	uint8_t getID();

protected:
	Pin pin;
	uint8_t id;
	double *table;
	int table_size;
	double *value;
};
