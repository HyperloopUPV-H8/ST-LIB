/*
 * EXTI.hpp
 *
 *  Created on: Nov 5, 2022
 *      Author: alejandro 
 */

#pragma once
#include "PinModel/Pin.hpp"

#ifdef HAL_EXTI_MODULE_ENABLED
#define GPIO_PORT GPIOE

class ExternalInterrupt {
public:
	class Instance {
	public:
		IRQn_Type interrupt_request_number;
		function<void()> action = nullptr;
		bool is_on = true;

		Instance() = default;
		Instance(IRQn_Type interrupt_request_number);
	};

	static map<uint8_t, Pin> service_ids;
	static map<uint16_t, Instance> instances;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin& pin, function<void()>&& action);
	static void start();
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static optional<bool> get_pin_value(uint8_t id);
};
#endif
