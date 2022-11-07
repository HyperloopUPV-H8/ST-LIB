
/*
 * DigitalSensor.hpp
 *
 *  Created on: Nov 2, 2022
 *      Author: ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_LOW/Sensors/Sensor/Sensor.hpp"


class DigitalSensor : public Sensor<std::optional<PinState>>::Sensor{
public:
	DigitalSensor(Pin pin);
	virtual void exti_interruption();
	void read(std::optional<PinState> &value);
	uint8_t getID();

private:
	uint8_t id;
	Pin pin;

};

