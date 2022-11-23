/*
 * DigitalOutput.cpp
 *
 *  Created on: Oct 18, 2022
 *      Author: stefan
 */

#include "DigitalOutput/DigitalOutput.hpp"

uint8_t DigitalOutput::inscribe(Pin& pin){
        Pin::inscribe(pin, OUTPUT);
		uint8_t id = DigitalOutput::id_manager.front();
		DigitalOutput::service_ids[id] = pin;
		DigitalOutput::id_manager.pop_front();
		return id;
}

void DigitalOutput::turn_off(uint8_t id){
	if (not DigitalOutput::service_ids.contains(id))
		return;

	Pin pin = DigitalOutput::service_ids[id];
	HAL_GPIO_WritePin(pin.port, pin.gpio_pin, (GPIO_PinState)PinState::OFF);
}

void DigitalOutput::turn_on(uint8_t id){
	if (not DigitalOutput::service_ids.contains(id))
		return;

	Pin pin = DigitalOutput::service_ids[id];
	HAL_GPIO_WritePin(pin.port, pin.gpio_pin, (GPIO_PinState)PinState::ON);
}

void DigitalOutput::set_pin_state(uint8_t id, PinState state){
	if (not DigitalOutput::service_ids.contains(id))
		return;

	Pin pin = DigitalOutput::service_ids[id];
	HAL_GPIO_WritePin(pin.port, pin.gpio_pin, (GPIO_PinState) state);
}
