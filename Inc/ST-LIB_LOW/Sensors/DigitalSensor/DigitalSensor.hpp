
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
	virtual void exti_interruption();
	void read();
	uint8_t getID();

protected:
	Pin pin;
	uint8_t id;
	PinState *value;
};

