/*
 * DualPhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPWM/DualPWM.hpp"

class DualPhasedPWM : public DualPWM{
private:
	float phase;
	DualPhasedPWM() = default;
public:
	DualPhasedPWM(Pin& pin, Pin& pin_negated);
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);
	void set_phase(float phase);
	float get_phase()const;
};
