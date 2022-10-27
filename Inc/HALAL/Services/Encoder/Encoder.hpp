/*
 * Encoder.hpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"
#include "../../../C++Utilities/CppUtils.hpp"


class Encoder {
public:
	static optional<uint8_t> register_encoder(Pin pin);

	static void turn_on_encoder(uint8_t id);

	static void turn_off_encoder(uint8_t id);

	static void get_encoder_value(uint8_t id);
};
