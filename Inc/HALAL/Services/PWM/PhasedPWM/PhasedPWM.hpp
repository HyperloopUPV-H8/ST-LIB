/*
 * PhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PWM/PWM.hpp"

class PhasedPWM : virtual public PWM {
private:
	int16_t phase;
//	uint16_t duty_cycle;

public:
	void set_duty_cycle(uint8_t duty_cycle);
	void set_phase(int8_t phase);

	PhasedPWM(Pin& pin);
};
