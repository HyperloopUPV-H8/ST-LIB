
/*
 * DigitalSensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"


class DigitalSensor{
public:
	DigitalSensor(Pin pin);
	virtual void exti_interruption();
	void read(auto *value);
	uint8_t getID();

private:
	uint8_t id;
	Pin pin;

};

