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
 * @brief Encoder service class. Abstracts the use of the encoder with the HAL library.
 *
 */
class Encoder {
public:
	static forward_list<uint8_t> ID_manager;
	static map<pair<Pin, Pin>, TIM_HandleTypeDef*> pin_timer_map;
	static map<uint8_t, pair<Pin, Pin>> service_IDs;

	/**
	 * @brief This method registers a new encoder
	 *
	 * @param pin1	First pin of the encoder
	 * @param pin2 	Second pin of the encoder
	 *
	 * @retval optional<uint8_t> Id of the service or empty if the pin pair is not valid
	 */
	static optional<uint8_t> register_encoder(Pin& pin1, Pin& pin2);

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
