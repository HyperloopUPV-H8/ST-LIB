/*
 * DigitalOut.hpp
 *
 *  Created on: Oct 17, 2022
 *      Author: stefan
 */
#pragma once
#include "ST-LIB-CORE.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED
class DigitalInput{
public:
	static map<uint8_t,Pin> service_ids;
	static forward_list<uint8_t> id_manager;

	static uint8_t inscribe(Pin& pin);
	static optional<PinState> read_pin_state(uint8_t id);
};
#endif
