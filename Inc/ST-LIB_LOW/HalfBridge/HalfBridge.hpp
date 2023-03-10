/*
 * HalfBridge.hpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#pragma once

#include "Time/Time.hpp"
#include "DigitalOutputService/DigitalOutputService.hpp"
#include "PWM/PhasedPWM/PhasedPWM.hpp"
#include "PWM/DualPhasedPWM/DualPhasedPWM.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class HalfBridge {
public:
	union PWMType {
		PhasedPWM p;
		DualPhasedPWM dp;

		PWMType() {new(&p) PhasedPWM();}
	};


	HalfBridge() = default;
	HalfBridge(Pin& positive_pwm_pin, Pin& negative_pwm_pin, Pin& enable_pin);
	HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin, Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin,
			Pin& enable_pin);

	void turn_on();
	void turn_off();
	void set_duty_cycle(int8_t duty_cycle);

private:
	bool is_dual;

	PWMType positive_pwm;
	PWMType negative_pwm;
	uint8_t enable;
};
