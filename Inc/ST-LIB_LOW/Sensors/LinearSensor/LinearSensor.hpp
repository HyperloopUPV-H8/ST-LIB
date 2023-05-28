/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ADC/ADC.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class LinearSensor{
public:
	LinearSensor() = default;
	LinearSensor(Pin &pin, double slope, double offset, double *value);
	LinearSensor(Pin &pin, double slope, double offset, double &value);
	void read();
	uint8_t get_id();

protected:
	uint8_t id;
	double slope;
	double offset;
	double *value;
};
