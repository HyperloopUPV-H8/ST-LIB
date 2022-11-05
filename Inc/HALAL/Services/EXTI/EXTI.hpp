/*
 * EXTI.hpp
 *
 *  Created on: Nov 5, 2022
 *      Author: aleja
 */

#pragma once
#include "ST-LIB.hpp"

struct GPIOValue {
	uint16_t gpio;
	bool value;
};

class ExtI {
public:
	static map<uint8_t, Pin> service_ids;
	static map<Pin, GPIOValue> pin_gpio_map;

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_exti(Pin& pin);
	static void unregister_exti(uint8_t id);
	static void turn_on_exti(uint8_t id);
	static void turn_off_exti(uint8_t id);
	bool* get_pin_value(pin);
};
