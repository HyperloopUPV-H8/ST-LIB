/*
 * Encoder.hpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#pragma once

#include "ST-LIB.hpp"

struct DoublePin {
	Pin* pin1;
	Pin* pin2;

	bool operator<(const DoublePin& other) const {
		if (pin1 < other.pin1) {
			return true;
		}else if(pin1 == pin2){
			if (pin2 < other.pin2) {
				return true;
			}
		}
		return false;
	}

	bool operator==(const DoublePin& other) const {
		return pin1 == other.pin1 && pin2 == other.pin2;
	}
};

struct DoubleTimerChannel {
	TIM_HandleTypeDef* timer;
	unsigned int channel1;
	unsigned int channel2;
};

class Encoder {
public:
	static forward_list<uint8_t> ID_manager;
	static map<DoublePin, DoubleTimerChannel> pin_timer_map;
	static map<uint8_t, DoublePin> service_IDs;

	static optional<uint8_t> register_encoder(Pin pin1, Pin pin2);

	static void turn_on_encoder(uint8_t id);

	static void turn_off_encoder(uint8_t id);

	static void reset_encoder(uint8_t id);

	static optional<uint32_t> get_encoder_counter(uint8_t id);

	static optional<bool> get_encoder_direction(uint8_t id);
};
