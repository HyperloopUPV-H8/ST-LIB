/*
 * DigitalInput.cpp
 *
 *  Created on: Oct 18, 2022
 *      Author: stefan
 */

#include "DigitalInput/DigitalInput.hpp"

uint8_t DigitalInput::inscribe(Pin& pin){
		Pin::inscribe(pin, INPUT);
		uint8_t id = DigitalInput::id_manager.front();
		DigitalInput::service_ids[id] = pin;
		DigitalInput::id_manager.pop_front();
		return id;
}

optional<PinState> DigitalInput::read_pin_state(uint8_t id){
	if (not DigitalInput::service_ids.contains(id))
		return nullopt;

	Pin pin = DigitalInput::service_ids[id];
	return (PinState)HAL_GPIO_ReadPin(pin.port, pin.gpio_pin);
}
