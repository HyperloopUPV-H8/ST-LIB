/*
 * DigitalOutput.hpp
 *
 *  Created on: 1 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "DigitalOutputservice/DigitalOutputservice.hpp"

class DigitalOutput {
public:
	DigitalOutput(Pin& pin);

	void turn_on();
	void turn_off();
	void set_pin_state(PinState state);
private:
	uint8_t id;
	Pin pin;
};


