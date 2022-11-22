
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 9, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"
#define counter_distance 0.025
#define clock_frequency 300000000


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
	double last_speed;
	const int n_frames = 5; //Cannot be lower than one
	const double frame_size = 0.01;
	uint32_t counters[n_frames];
	uint64_t times[n_frames];
};
