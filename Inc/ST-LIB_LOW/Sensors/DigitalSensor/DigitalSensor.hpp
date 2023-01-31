
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
	DigitalSensor(Pin &pin, PinState *value);
	DigitalSensor(Pin &pin, PinState &value);
	void exti_interruption(std::function<void()> &&action);
	void read();
	uint8_t get_id();

protected:
	Pin &pin;
	uint8_t id;
	uint8_t exti_id;
	PinState *value;
};


