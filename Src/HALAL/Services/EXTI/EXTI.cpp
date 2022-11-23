/*
 * EXTI.cpp
 *
 *  Created on: Nov 5, 2022
 *      Author: alejandro 
 */

#include "EXTI/EXTI.hpp"

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
	ExternalInterrupt::Instance& exti = ExternalInterrupt::instances[GPIO_Pin];
	if (exti.is_on) {
		exti.action();
	}
}

optional<uint8_t> ExternalInterrupt::inscribe(Pin& pin, function<void()>&& action) {
	if (not instances.contains(pin.gpio_pin)) {
		return nullopt;
	}

	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	id_manager.pop_front();

	service_ids[id] = pin;
	instances[pin.gpio_pin].action = action;

	return id;
}

void ExternalInterrupt::turn_on(uint8_t id) {
	if (not service_ids.contains(id)) {
		return; //TODO: error handler
	}

	Instance& instance = instances[service_ids[id].gpio_pin];
	instance.is_on = true;
}

optional<bool> ExternalInterrupt::get_pin_value(uint8_t id) {
	if (not service_ids.contains(id)) {
		return nullopt; //TODO: error handler
	}

	Pin& pin = service_ids[id];
	return HAL_GPIO_ReadPin(GPIO_PORT, pin.gpio_pin);
}
