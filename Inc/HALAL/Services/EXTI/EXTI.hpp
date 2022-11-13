/*
 * EXTI.hpp
 *
 *  Created on: Nov 5, 2022
 *      Author: aleja
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "ST-LIB.hpp"

struct EXTIdata {
	uint16_t gpio;
	function<void()>* action = nullptr;
	bool is_on = true;
};

class ExternalInterrupt {
public:
	static map<uint8_t, Pin> service_ids;
	static map<Pin, EXTIdata> pin_gpio_map;

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_exti(Pin& pin, function<void()>* action);
	static void unregister_exti(uint8_t id);
	static void turn_on_exti(uint8_t id);
	static void turn_off_exti(uint8_t id);
	static bool get_pin_value(uint8_t id);
};
