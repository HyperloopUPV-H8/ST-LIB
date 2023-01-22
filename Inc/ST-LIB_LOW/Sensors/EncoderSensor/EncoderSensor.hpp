
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 9, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#define COUNTER_DISTANCE_IN_METERS 0.025
#define NANO_SECOND 1000000000.0
#define N_FRAMES 5
#define FRAME_SIZE_IN_SECONDS 0.01
#define START_COUNTER 32768
#define CLOCK_MAX_VALUE 4294967295 //here goes the tim2 counter period



class EncoderSensor{
public:
	EncoderSensor(Pin pin1, Pin pin2, double* position, double* speed, double* acceleration);
	void start();
	void read();
	uint8_t get_id();
	void get_arrays(double ret[][N_FRAMES]);

protected:
	Pin pin1;
	Pin pin2;
	uint8_t id;
	double* position;
	double* speed;
	double* acceleration;
	double time;
	double positions[N_FRAMES];
	double times[N_FRAMES];
	double speeds[N_FRAMES];
	uint64_t last_clock_time;

private:
	void update_arrays();
};
