/*
 * LookupSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/LinearSensor/AnalogSensor.hpp"


class LookupSensor : public AnalogSensor::AnalogSensor{
public:
	LookupSensor(Pin pin, double *table);
	void read(double &value);
	uint8_t getID();

private:
	uint8_t id;
	double *table;
	Pin pin;

};
