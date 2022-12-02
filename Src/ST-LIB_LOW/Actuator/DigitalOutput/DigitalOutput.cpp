/*
 * DigitalOutput.cpp
 *
 *  Created on: 1 dic. 2022
 *      Author: aleja
 */

#include "Actuator/DigitalOutput/DigitalOutput.hpp"

DigitalOutput::DigitalOutput(Pin& pin) : pin(pin) {
	optional<uint8_t> try_id = DigitalOutputservice::inscribe(pin);
		if (not try_id) {
			//TODO: error handler
		}

		id = try_id.value();
}

void DigitalOutput::turn_on() {
	DigitalOutputservice::turn_on(id);
}

void DigitalOutput::turn_off() {
	DigitalOutputservice::turn_off(id);
}

void DigitalOutput::set_pin_state(PinState state) {
	DigitalOutputservice::set_pin_state(id, state);
}
