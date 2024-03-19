/*
 * DualPhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPWM/DualPWM.hpp"

class DualPhasedPWM : public PhasedPWM {
private:
	DualPhasedPWM() = default;
public:
	DualPhasedPWM(Pin& pin, Pin& pin_negated);
	void set_duty_cycle(float duty);
	void turn_on();
};
