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
		//TODO: Error Handler
	}
	HalfBridge::positive_pwm = positive_pwm_id.value();

	optional<uint8_t> negative_pwm_id = PWMservice::inscribe(negative_pwm_pin);
	if (not negative_pwm_id) {
		//TODO: Error Handler
	}
	HalfBridge::negative_pwm = negative_pwm_id.value();

	optional<uint8_t> enable_id = DigitalOutputservice::inscribe(enable_pin);
	if (not enable_id) {
		//TODO: Error Handler
	}
	HalfBridge::enable = enable_id.value();

}

HalfBridge::HalfBridge(Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) {
	optional<uint8_t> positive_pwm_id = PWMservice::inscribe_dual(positive_pwm_pin, positive_pwm_negated_pin);
	if (not positive_pwm_id) {
		//TODO: Error Handler
	}
	HalfBridge::positive_pwm = positive_pwm_id.value();

	optional<uint8_t> negative_pwm_id = PWMservice::inscribe_dual(negative_pwm_pin, negative_pwm_negated_pin);
	if (not negative_pwm_id) {
		//TODO: Error Handler
	}
	HalfBridge::negative_pwm = negative_pwm_id.value();

	optional<uint8_t> enable_id = DigitalOutputservice::inscribe(enable_pin);
	if (not enable_id) {
		//TODO: Error Handler
	}
	HalfBridge::enable = enable_id.value();
}

void HalfBridge::turn_on() {
	PWMservice::turn_on(positive_pwm);
	PWMservice::turn_on(negative_pwm);
	DigitalOutputservice::turn_on(enable); // enable al final para evitar ruido
}

void HalfBridge::turn_off() {
	DigitalOutputservice::turn_off(enable); // enable al principio para evitar ruido
	PWMservice::turn_off(positive_pwm);
	PWMservice::turn_off(negative_pwm);
}

void HalfBridge::set_duty_cycle(int8_t duty_cycle) {
	if (duty_cycle >= 0) {
		PWMservice::set_duty_cycle(negative_pwm, 0);
		PWMservice::set_duty_cycle(positive_pwm, duty_cycle);
	}
	else {
		PWMservice::set_duty_cycle(positive_pwm, 0);
		PWMservice::set_duty_cycle(negative_pwm, duty_cycle);
	}
}
