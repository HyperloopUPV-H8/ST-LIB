/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


class LinearSensor{
public:
	LinearSensor(Pin pin, double slope, double offset, double *value);
	void read();
	uint8_t get_id();

protected:
	Pin pin;
	uint8_t id;
	double slope;
	double offset;
	double *value;
};
