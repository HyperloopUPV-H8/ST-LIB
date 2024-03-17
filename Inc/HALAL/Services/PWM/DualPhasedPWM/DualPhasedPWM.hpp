/*
 * DualPhasedPWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPWM/DualPWM.hpp"

class DualPhasedPWM : public PhasedPWM, public DualPWM {
private:
	DualPhasedPWM() = default;
public:
	DualPhasedPWM(Pin& pin, Pin& pin_negated);

};
