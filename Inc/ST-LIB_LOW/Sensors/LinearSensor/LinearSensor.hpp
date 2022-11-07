/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


class LinearSensor : public AnalogSensor{
public:
	LinearSensor(Pin pin, double slope, double offset);
	void read(double &value);
	uint8_t getID();

private:
	uint8_t id;
	Pin pin;

};
