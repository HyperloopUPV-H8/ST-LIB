/*
 * Encoder.hpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#pragma once
#ifdef HAL_TIM_MODULE_ENABLED
#include "ST-LIB.hpp"

/**
 * @struct DoublePin
 * @brief Estruct that contains a pair of pins
 *
 * @var DoublePin::pin1
 * The first pin of the structure
 * @var DoublePin::pin2
 * The second pin of the structure
 *
 * */
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

	bool operator<(const DoublePin* other) const {
		if (*pin1 < *other->pin1) {
			return true;
		}else if(*pin1 == *other->pin1){
			if (*pin2 < *other->pin2) {
				return true;
			}
		}
		return false;
	}

	bool operator==(const DoublePin& other) const {
		return pin1 == other.pin1 && pin2 == other.pin2;
	}

	bool operator==(const DoublePin* other) const {
			return *pin1 == *other->pin1 && *pin2 == *other->pin2;
		}
};

struct EncoderPinTimer{
	Pin pin1;
	Pin pin2;
	TIM_HandleTypeDef* timer;
};

/**
 * @brief Encoder service class for abstracting the use of the encoder with the HAL library.
 *
 */
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
	static optional<uint8_t> register_encoder(Pin* pin1, Pin* pin2);

	/**
	 * @brief Starts the timer of the encoder
	 *
	 * @param id Id of the encoder
	 */
	static void turn_on_encoder(uint8_t id);

	/**
	 * @brief Stop the timer of the encoder
	 *
	 * @param id Id of the encoder
	 */
	static void turn_off_encoder(uint8_t id);

	/**
	 * @brief Resets the encoder by setting the CNT register to 0
	 *
	 * @param id Id of the encoder
	 */
	static void reset_encoder(uint8_t id);

	/**
	 * @brief Get the CNT value of the encoder
	 *
	 * @param id Id of the encoder
	 * @return optional<uint32_t> CNT value if the id is valid, empty if not
	 */
	static optional<uint32_t> get_encoder_counter(uint8_t id);

	/**
	 * @brief Get the encoder direction
	 *
	 * @param id Id
	 * @return optional<bool> Encoder direction if id is valid, empty if not
	 */
	static optional<bool> get_encoder_direction(uint8_t id);
};
#endif
