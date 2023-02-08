
/*
 * SensorInterrupt.hpp
 *
 *  Created on: Jan 31, 2023
 *      Author: ricardo
 */

#pragma once
#include "EXTI/EXTI.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class SensorInterrupt{
public:
	SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState *value);
	SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState &value);
	void read();
	uint8_t get_id();

protected:
	Pin &pin;
	uint8_t id;
	PinState *value;
};
