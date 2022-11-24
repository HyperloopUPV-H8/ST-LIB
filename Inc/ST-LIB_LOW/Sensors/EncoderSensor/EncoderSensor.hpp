
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 9, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"
#define counter_distance 0.025 //meters
#define clock_frequency 300000000
#define n_frames 5 //number of time frames captured (compares first to last)
#define frame_size 0.01 //seconds



class EncoderSensor : public Sensor<double>::Sensor{
public:
	EncoderSensor(Pin pin1, Pin pin2, double* position, double* speed, double* acceleration);
	void start();
	void read();
	uint8_t get_id();

protected:
	Pin pin1;
	Pin pin2;
	uint8_t id;
	double* position;
	double* speed;
	double* acceleration;
	double last_speed;
	uint32_t counters[n_frames];
	uint64_t times[n_frames];
};
