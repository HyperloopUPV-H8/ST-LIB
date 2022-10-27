/*
 * Encoder.hpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"
#include "../../../C++Utilities/CppUtils.hpp"

struct DoubleTimerChannel {
	TIM_HandleTypeDef* timer;
	unsigned int channel1;
	unsigned int channel2;
};

class Encoder {
public:
	static forward_list<uint8_t> ID_manager;
	static map<pair<Pin, Pin>, DoubleTimerChannel> pin_timer_map;
	static map<uint8_t, pair<Pin, Pin>> service_IDs;

	static optional<uint8_t> register_encoder(Pin pin);

	static void turn_on_encoder(uint8_t id);

	static void turn_off_encoder(uint8_t id);

	static void rest_encoder(uint8_t id);

	static optional<uint32_t> get_encoder_counter(uint8_t id);

	static optional<bool> get_encoder_direction(uint8_t id);
};
