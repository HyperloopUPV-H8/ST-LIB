/*
 * PhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PWM/PWM.hpp"

class PhasedPWM :  public PWM {
protected:
	float phase;
	PhasedPWM() = default;

public:
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);
	void set_phase(float phase);

	PhasedPWM(Pin& pin);

};
