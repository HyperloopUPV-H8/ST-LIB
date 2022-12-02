/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */
#pragma once
#include "PinModel/Pin.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED
class DigitalOutputService{
public:
	static map<uint8_t,Pin> service_ids;
	static forward_list<uint8_t> id_manager;

	static uint8_t inscribe(Pin& pin);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static void set_pin_state(uint8_t id, PinState state);
};
#endif
