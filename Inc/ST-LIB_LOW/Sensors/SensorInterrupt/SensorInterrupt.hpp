
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
	SensorInterrupt() = default;
	SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState *value, ExternalInterrupt::TRIGGER trigger = ExternalInterrupt::RISING);
	SensorInterrupt(Pin &pin, std::function<void()> &&action, PinState &value, ExternalInterrupt::TRIGGER trigger = ExternalInterrupt::RISING);
	void read();
	uint8_t get_id();

protected:
	uint8_t id;
	PinState *value;
};
