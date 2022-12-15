/*
 * Encoder.hpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#pragma once


#include "PinModel/Pin.hpp"
#include "TimerPeripheral/TimerPeripheral.hpp"
#include "C++Utilities/CppUtils.hpp"


#ifdef HAL_TIM_MODULE_ENABLED
/**
 * @brief Encoder service class. Abstracts the use of the encoder with the HAL library.
 *
 */
class Encoder {
public:
	static uint8_t id_counter;
	static map<pair<Pin, Pin>, TimerPeripheral*> pin_timer_map;
	static map<uint8_t, pair<Pin, Pin>> registered_encoder;

	/**
	 * @brief This method registers a new encoder
	 *
	 * @param pin1	First pin of the encoder
	 * @param pin2 	Second pin of the encoder
	 *
	 * @retval optional<uint8_t> Id of the service or empty if the pin pair is not valid
	 */
	static optional<uint8_t> inscribe(Pin& pin1, Pin& pin2);

	static void start();

	/**
	 * @brief Starts the timer of the encoder
	 *
	 * @param id Id of the encoder
	 */
	static void turn_on(uint8_t id);

	/**
	 * @brief Stop the timer of the encoder
	 *
	 * @param id Id of the encoder
	 */
	static void turn_off(uint8_t id);

	/**
	 * @brief Resets the encoder by setting the CNT register to 0
	 *
	 * @param id Id of the encoder
	 */
	static void reset(uint8_t id);

	/**
	 * @brief Get the CNT value of the encoder
	 *
	 * @param id Id of the encoder
	 * @return optional<uint32_t> CNT value if the id is valid, empty if not
	 */
	static optional<uint32_t> get_counter(uint8_t id);

	/**
	 * @brief Get the encoder direction
	 *
	 * @param id Id
	 * @return optional<bool> Encoder direction if id is valid, empty if not
	 */
	static optional<bool> get_direction(uint8_t id);

	static void init(TimerPeripheral* encoder);
};
#endif
