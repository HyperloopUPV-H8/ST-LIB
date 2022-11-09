/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/LinearSensor/AnalogSensor.hpp"


class LinearSensor : public AnalogSensor::AnalogSensor{
public:
	LinearSensor(Pin pin, double slope, double offset, double destination, double *value);
	void read();
	uint8_t getID();

private:
	Pin pin;
	uint8_t id;
	double slope;
	double offset;
	double destination;
	double *value;

};
