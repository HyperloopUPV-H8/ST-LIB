/*
 * PWM.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: aleja
 */

#pragma once

#include "PWMService/PWMService.hpp"

class PWM {
public:
	PWM() = default;
	PWM(Pin& pin);
	PWM(Pin& pin, Pin& pin_negated);

	void turn_on();
	void turn_off();
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);

private:
	uint8_t id;
	Pin pin;
	Pin pin_negated;
};
