
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 9, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"


class EncoderSensor : public Sensor<double>::Sensor{
public:
	EncoderSensor(Pin pin1, Pin pin2, double *value);
	EncoderSensor(Pin pin1, Pin pin2, Pin pin3, double *value);
	void read();
	uint8_t getID();

private:
	uint8_t id;
	Pin pin1;
	Pin pin2;
	Pin pin3;

};
