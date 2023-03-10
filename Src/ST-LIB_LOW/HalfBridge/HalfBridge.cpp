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

	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
	if (not enable_id) {
		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
	}
	HalfBridge::enable = enable_id.value();
}

void HalfBridge::turn_on() {
	positive_pwm.turn_on();
	negative_pwm.turn_on();
	DigitalOutputService::turn_on(enable); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	DigitalOutputService::turn_off(enable); // enable al principio para evitar ruido
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
	positive_pwm.set_phase(phase);
	negative_pwm.set_phase(-phase);
}
