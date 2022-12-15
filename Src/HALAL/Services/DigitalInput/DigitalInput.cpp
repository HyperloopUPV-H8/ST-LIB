/*
 * DigitalInput.cpp
 *
 *  Created on: Oct 18, 2022
 *      Author: stefan
 */

#include "DigitalInput/DigitalInput.hpp"

uint8_t id_counter = 0;
map<uint8_t,Pin> DigitalInput::service_ids = {};

uint8_t DigitalInput::inscribe(Pin& pin){
		Pin::inscribe(pin, INPUT);
		DigitalInput::service_ids[id_counter] = pin;
		return id_counter++;
}

optional<PinState> DigitalInput::read_pin_state(uint8_t id){
	if (not DigitalInput::service_ids.contains(id))
		return nullopt;

	Pin pin = DigitalInput::service_ids[id];
	return (PinState)HAL_GPIO_ReadPin(pin.port, pin.gpio_pin);
}
