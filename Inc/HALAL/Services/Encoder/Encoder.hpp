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
		}else if(pin1 == other.pin1){
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

class Encoder {
public:
	static forward_list<uint8_t> ID_manager;
	static map<DoublePin, TIM_HandleTypeDef*> pin_timer_map;
	static map<uint8_t, DoublePin> service_IDs;

	/**
	 * @brief This method register a new encoder
	 *
	 * @param pin1	First pin of the encoder
	 * @param pin2 	Second pin of the encoder
	 *
	 * @retval optional<uint8_t> Id of the service or empty if the pin pair is not valid
	 */
	static optional<uint8_t> register_encoder(Pin pin1, Pin pin2, TIM_HandleTypeDef* timer);

	static void turn_on_encoder(uint8_t id);

	static void turn_off_encoder(uint8_t id);

	static void reset_encoder(uint8_t id);

	static optional<uint32_t> get_encoder_counter(uint8_t id);

	static optional<bool> get_encoder_direction(uint8_t id);
};
