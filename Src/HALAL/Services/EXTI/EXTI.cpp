/*
 * EXTI.cpp
 *
 *  Created on: Nov 5, 2022
 *      Author: alejandro 
 */

#include "EXTI/EXTI.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

uint8_t ExternalInterrupt::id_counter = 0;
map<uint8_t, Pin> ExternalInterrupt::service_ids = {};

ExternalInterrupt::Instance::Instance(IRQn_Type interrupt_request_number) :
		interrupt_request_number(interrupt_request_number) {}

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

	Pin::inscribe(pin, EXTERNAL_INTERRUPT);

	service_ids[id_counter] = pin;
	instances[pin.gpio_pin].action = action;

	return id_counter++;
}

void ExternalInterrupt::start() {
	for(auto id_instance : instances) {
		Instance& instance = id_instance.second;
		  HAL_NVIC_SetPriority(instance.interrupt_request_number, 0, 0);
		  HAL_NVIC_EnableIRQ(instance.interrupt_request_number);
	}
}

void ExternalInterrupt::turn_on(uint8_t id) {
	if (not service_ids.contains(id)) {
		ErrorHandler("ID %d is not registered as an active instance", id);
		return;
	}

	Instance& instance = instances[service_ids[id].gpio_pin];
	instance.is_on = true;
}

optional<bool> ExternalInterrupt::get_pin_value(uint8_t id) {
	if (not service_ids.contains(id)) {
		ErrorHandler("ID %d is not registered as an active instance", id);
		return nullopt;
	}

	Pin& pin = service_ids[id];
	return HAL_GPIO_ReadPin(GPIO_PORT, pin.gpio_pin);
}
