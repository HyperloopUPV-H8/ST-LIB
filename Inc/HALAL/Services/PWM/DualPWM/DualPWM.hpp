/*
 * DualPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PWM/PWM.hpp"

class DualPWM{
private:
	DualPWM() = default;
	TimerPeripheral* peripheral;
	uint32_t channel;
	float duty_cycle;
	uint32_t frequency;
	bool is_on = false;
public:
	DualPWM(Pin& pin, Pin& pin_negated);

	void turn_on();
	void turn_on_positive();
	void turn_on_negated();
	void turn_off();
	void turn_off_positive();
	void turn_off_negated();
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);
	uint32_t get_frequency();
	float get_duty_cycle();

};
