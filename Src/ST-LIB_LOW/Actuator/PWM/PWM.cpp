/*
 * PWM.cpp
 *
 *  Created on: 30 nov. 2022
 *      Author: aleja
 */

#include "Actuator/PWM/PWM.hpp"

PwmInstance::PwmInstance(Pin pin) : pin(pin) {
	optional<uint8_t> try_id = PWM::inscribe(pin);
	if (not try_id) {
		//TODO: error handler
	}

	id = try_id.value();
}
PwmInstance::PwmInstance(Pin pin, Pin pin_negated) : pin(pin), pin_negated(pin_negated) {
	optional<uint8_t> try_id = PWM::inscribe_dual(pin, pin_negated);
	if (not try_id) {
		//TODO: error handler
	}

	id = try_id.value();
}

void PwmInstance::turn_on() {
	PWM::turn_on(id);
}

void PwmInstance::turn_off() {
	PWM::turn_off(id);
}

void PwmInstance::set_duty_cycle(uint8_t duty_cycle) {
	PWM::set_duty_cycle(id, duty_cycle);
}
