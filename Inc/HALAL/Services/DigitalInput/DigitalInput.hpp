/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"

class DigitalInput{
public:
	static map<uint8_t,Pin> serviceIDs;
	static forward_list<uint8_t> IDmanager;

	static uint8_t register_digital_input(Pin& pin);
	static void unregister_digital_input(uint8_t id);
	static PinState read_pin_state(uint8_t id);
};


