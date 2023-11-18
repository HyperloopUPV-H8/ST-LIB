
/*
 * DigitalSensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "DigitalInputService/DigitalInputService.hpp"
#include "EXTI/EXTI.hpp"
#include "ErrorHandler/ErrorHandler.hpp"


class DigitalSensor{
public:
	DigitalSensor() = default;
	DigitalSensor(Pin &pin, PinState *value);
	DigitalSensor(Pin &pin, PinState &value);
	void read();
	uint8_t get_id();
	uint8_t get_value();
protected:
	uint8_t id;
	PinState *value;
};


