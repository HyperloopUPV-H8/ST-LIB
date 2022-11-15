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
		function<void()> action = nullptr;
		bool is_on = true;
	};

	static map<uint8_t, Pin> service_ids;
	static map<uint16_t, Instance> instances;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_(Pin& pin, function<void()>&& action);
	static void unregister(uint8_t id);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static bool get_pin_value(uint8_t id);
};
