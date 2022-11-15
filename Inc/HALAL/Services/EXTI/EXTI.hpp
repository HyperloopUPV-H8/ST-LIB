/*
 * EXTI.hpp
 *
 *  Created on: Nov 5, 2022
 *      Author: aleja
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "ST-LIB.hpp"

#define GPIO_PORT GPIOE

class ExternalInterrupt {
public:
	class Instance {
	public:
		uint8_t id;
		Pin pin;
		uint16_t gpio;
		function<void()>* action = nullptr;
		bool is_on = true;

		Instance() = default;
		Instance(Pin _pin, uint16_t _gpio);
	};

	static map<uint8_t, Instance> instances;
	static map<Pin, Instance> instances_data;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_exti(Pin& _pin, function<void()>* _action);
	static void unregister_exti(uint8_t id);
	static void turn_on_exti(uint8_t id);
	static void turn_off_exti(uint8_t id);
	static bool get_pin_value(uint8_t id);
};
