
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
	virtual void read();
	virtual uint8_t getID();

protected:
	Pin pin;
	uint8_t id;
};
