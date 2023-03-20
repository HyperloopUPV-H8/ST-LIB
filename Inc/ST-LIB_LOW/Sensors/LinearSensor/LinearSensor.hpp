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
	LinearSensor(Pin &pin, float *value, float slope=1, float offset=0);
	LinearSensor(Pin &pin, float &value, float slope=1, float offset=0);
	void read();
	uint8_t get_id();

protected:
	Pin &pin;
	uint8_t id;
	float slope;
	float offset;
	float *value;
};
