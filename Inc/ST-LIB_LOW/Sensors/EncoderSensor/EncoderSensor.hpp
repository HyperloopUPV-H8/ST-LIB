
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
	EncoderSensor(Pin pin1, Pin pin2, double counter_distance, double* position, double* speed, double* acceleration);
	void read();
	uint8_t getID();

protected:
	Pin pin1;
	Pin pin2;
	uint8_t id;
	double counter_distance;
	double* position;
	double* speed;
	double* acceleration;
	uint32_t last_check;
	double last_speed;
};
