/*
 * PWM.cpp
 *
 *  Created on: 30 nov. 2022
 *      Author: aleja
 */

#include "Actuator/PWM/PWM.hpp"

PwmInstance::PWM(Pin pin) : pin(pin) {
	optional<uint8_t> try_id = PWMservice::inscribe(pin);
	if (not try_id) {
		//TODO: error handler
	}

	id = try_id.value();
}
PwmInstance::PWM(Pin pin, Pin pin_negated) : pin(pin), pin_negated(pin_negated) {
	optional<uint8_t> try_id = PWMservice::inscribe_dual(pin, pin_negated);
	if (not try_id) {
		//TODO: error handler
	}

	id = try_id.value();
}

void PWM::turn_on() {
	PWMservice::turn_on(id);
}

void PWM::turn_off() {
	PWMservice::turn_off(id);
}

void PWM::set_duty_cycle(uint8_t duty_cycle) {
	PWMservice::set_duty_cycle(id, duty_cycle);
}
