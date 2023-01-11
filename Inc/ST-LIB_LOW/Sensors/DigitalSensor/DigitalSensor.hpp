
/*
 * DigitalSensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"


class DigitalSensor : public Sensor<PinState>::Sensor{
public:
	DigitalSensor(Pin pin, PinState *value);
	void exti_interruption(std::function<void()> &&action);
	void start();
	void read();
	uint8_t get_id();

protected:
	Pin pin;
	uint8_t id;
	uint8_t exti_id;
	PinState *value;
};


