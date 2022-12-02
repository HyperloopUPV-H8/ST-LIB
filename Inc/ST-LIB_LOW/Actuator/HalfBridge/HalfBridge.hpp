/*
 * HalfBridge.hpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#pragma once

#include "Time/Time.hpp"
#include "Actuator/PWM/PWM.hpp"
#include "Actuator/DigitalOutput/DigitalOutput.hpp"

class HalfBridge {
public:
	HalfBridge() = default;
	HalfBridge(Pin& positive_voltage_pwm_pin, Pin& negative_voltage_pwm_pin, Pin& enable_pin);
	HalfBridge(Pin& positive_voltage_pwm_pin, Pin& positive_voltage_pwm_negated_pin, Pin& negative_voltage_pwm_pin, Pin& negative_voltage_pwm_negated_pin,
			Pin& enable_pin);

	void turn_on();
	void turn_off();
	void set_duty_cycle(int8_t duty_cycle);

private:
	PWM positive_voltage_pwm;
	PWM negative_voltage_pwm;
	DigitalOutput enable;
};
