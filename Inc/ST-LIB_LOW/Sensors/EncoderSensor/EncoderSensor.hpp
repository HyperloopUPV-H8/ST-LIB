
/*
 * AnalogSensor.hpp
 *
 *  Created on: Nov 9, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"
#define COUNTER_DISTANCE 0.025 //meters
#define CLOCK_FREQUENCY 1000000000.0
#define N_FRAMES 5 //number of time frames captured (compares first to last)
#define FRAME_SIZE 0.01 //seconds
#define START_COUNTER 32768



class EncoderSensor : public Sensor<double>::Sensor{
public:
	EncoderSensor(Pin pin1, Pin pin2, double* position, double* speed, double* acceleration);
	void start();
	void read();
	uint8_t get_id();
	double** get_arrays();

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
