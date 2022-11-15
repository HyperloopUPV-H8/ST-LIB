/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */
#pragma once
#include "ST-LIB.hpp"


class DigitalOutput{
public:
	static map<uint8_t,Pin> service_ids;
	static forward_list<uint8_t> id_manager;

	static uint8_t register_(Pin& pin);
	static void unregister(uint8_t id);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static void set_pin_state(uint8_t id, PinState state);
};
