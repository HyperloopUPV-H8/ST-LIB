
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
	EncoderSensor(Pin pin1, Pin pin2, double *value, double counter_distance);
	EncoderSensor(Pin pin1, Pin pin2, Pin pin3, double *value, double counter_distance);
	void read();
	uint8_t getID();

protected:
	Pin pin1;
	Pin pin2;
	Pin pin3;
	uint8_t id;
	double* value;
	double counter_distance;
	uint32_t last_check;
};
