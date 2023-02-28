/*
 * PWM.cpp
 *
 *  Created on: 30 nov. 2022
 *      Author: aleja
 */

#include "Actuator/PWM/PWM.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

PWM::PWM(Pin& pin) : pin(pin) {
	optional<uint8_t> try_id = PWMservice::inscribe(pin);
	if (not try_id) {
		ErrorHandler("Pin %s is not configured as a PWM in you configuration file", pin.to_string().c_str());
	}

	id = try_id.value();
}
PWM::PWM(Pin& pin, Pin& pin_negated) : pin(pin), pin_negated(pin_negated) {
	optional<uint8_t> try_id = PWMservice::inscribe_dual(pin, pin_negated);
	if (not try_id) {
		ErrorHandler("Pin %s and Pin %s are not configured as a dual PWM in you configuration file", pin.to_string().c_str(), pin.to_string().c_str());
	}

	id = try_id.value();
}

void PWM::turn_on() {
	PWMservice::turn_on(id);
}

void PWM::turn_off() {
	PWMservice::turn_off(id);
}

void PWM::set_duty_cycle(float duty_cycle) {
	PWMservice::set_duty_cycle(id, duty_cycle);
}

void PWM::set_frequency(uint32_t frequency) {
	PWMservice::set_frequency(id, frequency);
}
