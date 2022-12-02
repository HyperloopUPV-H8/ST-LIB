/*
 * HalfBridge.hpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Time/Time.hpp"
#include "Actuator/PWM/PWM.hpp"
#include "Actuator/DigitalOutput/DigitalOutput.hpp"

class HalfBridge {
public:
	HalfBridge(Pin positive_voltage_pwm_pin, Pin negative_voltage_pwm_pin, Pin enable_pin, uint8_t deadtime_in_clock_cycles = 45);
	HalfBridge(Pin positive_voltage_pwm_pin, Pin positive_voltage_pwm_negated_pin, Pin negative_voltage_pwm_pin, Pin negative_voltage_pwm_negated_pin,
			Pin enable_pin, uint8_t deadtime_in_clock_cycles = 45);

	void turn_on();
	void turn_off();
	void set_duty_cycle(int8_t duty_cycle);

private:
	const uint8_t deadtime_in_clock_cycles;

	PWM positive_voltage_pwm;
	PWM negative_voltage_pwm;
	DigitalOutput enable;
};
