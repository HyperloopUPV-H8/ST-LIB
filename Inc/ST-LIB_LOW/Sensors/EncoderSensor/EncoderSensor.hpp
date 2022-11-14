
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
	EncoderSensor(Pin pin1, Pin pin2, double* position, double* speed, double* acceleration);
	void read();
	uint8_t getID();

protected:
	Pin pin1;
	Pin pin2;
	uint8_t id;
	double* position;
	double* speed;
	double* acceleration;
	uint64_t last_time;
	double last_position;
	double last_speed;
	double counter_distance = 0.025;
	const int clock_frequency = 300000000;
};
