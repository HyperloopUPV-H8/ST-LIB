/*
 * HalfBridge.cpp
 *
 *  Created on: Dec 1, 2022
 *      Author: aleja
 */

#include "Actuator/HalfBridge/HalfBridge.hpp"

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& negative_pwm_pin, Pin& enable_pin){
	optional<uint8_t> positive_pwm_id = PWMservice::inscribe(positive_pwm_pin);
	if (not positive_pwm_id) {
		ErrorHandler("Unable to inscribe the HalfBridge positive PWM in %s", positive_pwm_pin.to_string());
	}
	HalfBridge::positive_pwm = positive_pwm_id.value();

	optional<uint8_t> negative_pwm_id = PWMservice::inscribe(negative_pwm_pin);
	if (not negative_pwm_id) {
		ErrorHandler("Unable to inscribe the HalfBridge negative PWM in %s", negative_pwm_pin.to_string());
	}
	HalfBridge::negative_pwm = negative_pwm_id.value();

	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
	if (not enable_id) {
		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
	}
	HalfBridge::enable = enable_id.value();

}

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) {
	optional<uint8_t> positive_pwm_id = PWMservice::inscribe_dual(positive_pwm_pin, positive_pwm_negated_pin);
	if (not positive_pwm_id) {
		ErrorHandler("Unable to inscribe the HalfBridge dual positive PWM in %s and %s", positive_pwm_pin.to_string(),positive_pwm_negated_pin.to_string());
	}
	HalfBridge::positive_pwm = positive_pwm_id.value();

	optional<uint8_t> negative_pwm_id = PWMservice::inscribe_dual(negative_pwm_pin, negative_pwm_negated_pin);
	if (not negative_pwm_id) {
		ErrorHandler("Unable to inscribe the HalfBridge dual negative PWM in %s and %s", negative_pwm_pin.to_string(), negative_pwm_negated_pin.to_string());
	}
	HalfBridge::negative_pwm = negative_pwm_id.value();

	optional<uint8_t> enable_id = DigitalOutputService::inscribe(enable_pin);
	if (not enable_id) {
		ErrorHandler("Unable to inscribe the HalfBridge enable pin in %s", enable_pin.to_string());
	}
	HalfBridge::enable = enable_id.value();
}

void HalfBridge::turn_on() {
	PWMservice::turn_on(positive_pwm);
	PWMservice::turn_on(negative_pwm);
	DigitalOutputService::turn_on(enable); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	DigitalOutputService::turn_off(enable); // enable al principio para evitar ruido
	PWMservice::turn_off(positive_pwm);
	PWMservice::turn_off(negative_pwm);
}

void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
	if (duty_cycle >= 0) {
		PWMservice::set_duty_cycle(positive_pwm, duty_cycle);
		PWMservice::set_duty_cycle(negative_pwm, 0);
	}
	else {
		PWMservice::set_duty_cycle(positive_pwm, 0);
		PWMservice::set_duty_cycle(negative_pwm, abs(duty_cycle));
	}
}
