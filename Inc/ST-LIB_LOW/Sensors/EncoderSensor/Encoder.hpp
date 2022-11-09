
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
	void EncoderSensor(Pin pin1, Pin pin2);
	void read(double &value);
	uint8_t getID();

private:
	double val;
	uint8_t id;
	Pin pin1;
	Pin pin2;
	Pin pin3;

};
