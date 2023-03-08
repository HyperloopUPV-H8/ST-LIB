/*
 * DualPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PWM/PWM.hpp"

class DualPWM : virtual public PWM {
public:
	DualPWM(Pin& pin, Pin& pin_negated);

	void turn_on();
	void turn_off();
};
