/*
 * PWM.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: aleja
 */

#pragma once

#include "Services/PWM/PWM.hpp"

class PwmInstance {
public:
	PwmInstance(Pin pin);
	PwmInstance(Pin pin, Pin pin_negated);

	void turn_on();
	void turn_off();
	void set_duty_cycle(uint8_t duty_cycle);

private:
	uint8_t id;
	Pin pin;
	Pin pin_negated;
};
