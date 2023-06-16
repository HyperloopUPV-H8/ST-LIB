/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include <HalfBridge/HalfBridge.hpp>

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) : is_dual(true) {
	positive_pwm = DualPhasedPWM(positive_pwm_pin, positive_pwm_negated_pin);
	negative_pwm = DualPhasedPWM(negative_pwm_pin, negative_pwm_negated_pin);

	HalfBridge::enable = DigitalOutputService::inscribe(enable_pin);
}

void HalfBridge::turn_on() {
	positive_pwm.turn_on();
	negative_pwm.turn_on();
	DigitalOutputService::turn_on(enable); // enable at the end to avoid noise
	DigitalOutputService::set_pin_state(enable, PinState::OFF);
}

void HalfBridge::turn_off() {
	DigitalOutputService::turn_off(enable); // enable at the start to avoid noise
	positive_pwm.turn_off();
	negative_pwm.turn_off();
}

void HalfBridge::set_duty_cycle(float duty_cycle) {
	positive_pwm.set_duty_cycle(duty_cycle);
	negative_pwm.set_duty_cycle(duty_cycle);
}

void HalfBridge::set_frequency(int32_t frequency) {
	positive_pwm.set_frequency(frequency);
	negative_pwm.set_frequency(frequency);
}

void HalfBridge::set_phase(float phase) {
	positive_pwm.set_phase(-phase);
	negative_pwm.set_phase(phase);
}

float HalfBridge::get_phase() {
	return negative_pwm.phase;
}
