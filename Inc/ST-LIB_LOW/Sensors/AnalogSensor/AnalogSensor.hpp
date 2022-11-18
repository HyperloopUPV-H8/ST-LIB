
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"


class AnalogSensor : public Sensor<double>::Sensor{
public:
	virtual void read() = 0;
	virtual uint8_t getID() = 0;

protected:
	Pin pin;
	uint8_t id;
	double *value;
};
