/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Actuator/HalfBridge/HalfBridge.hpp"

HalfBridge::HalfBridge(Pin positive_voltage_pwm_pin, Pin negative_voltage_pwm_pin, Pin enable_pin, uint8_t deadtime_in_clock_cycles) :
	deadtime_in_clock_cycles(deadtime_in_clock_cycles) {
	HalfBridge::positive_voltage_pwm = PWM(positive_voltage_pwm_pin);
	HalfBridge::negative_voltage_pwm = PWM(negative_voltage_pwm_pin);
	HalfBridge::enable = DigitalOutput(enable_pin);

}

HalfBridge::HalfBridge(Pin positive_voltage_pwm_pin, Pin positive_voltage_pwm_negated_pin,
		Pin negative_voltage_pwm_pin, Pin negative_voltage_pwm_negated_pin, Pin enable_pin, uint8_t deadtime_in_clock_cycles) :
			deadtime_in_clock_cycles(deadtime_in_clock_cycles){
	HalfBridge::positive_voltage_pwm = PWM(positive_voltage_pwm_pin, positive_voltage_pwm_negated_pin);
	HalfBridge::negative_voltage_pwm = PWM(negative_voltage_pwm_pin, negative_voltage_pwm_negated_pin);
	HalfBridge::enable = DigitalOutput(enable_pin);
}

void HalfBridge::turn_on() {
	positive_voltage_pwm.turn_on();
	negative_voltage_pwm.turn_on();
	enable.turn_on(); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	enable.turn_off(); // enable al principio para evitar ruido
	positive_voltage_pwm.turn_off();
	negative_voltage_pwm.turn_off();
}

void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
	if (duty_cycle >= 0) {
		negative_voltage_pwm.set_duty_cycle(0);
		positive_voltage_pwm.set_duty_cycle(duty_cycle);
	}
	else {
		positive_voltage_pwm.set_duty_cycle(0);
		negative_voltage_pwm.set_duty_cycle(duty_cycle);
	}
}
