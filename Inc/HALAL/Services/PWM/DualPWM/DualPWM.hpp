/*
 * DualPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PWM/PWM.hpp"

class DualPWM : virtual public PWM {
private:
	DualPWM() = default;
public:
	DualPWM(Pin& pin, Pin& pin_negated);

	void turn_on();
	void turn_on_positive();
	void turn_on_negated();
	void turn_off();
	void turn_off_positive();
	void turn_off_negated();

	friend class DualPhasedPWM;
};
