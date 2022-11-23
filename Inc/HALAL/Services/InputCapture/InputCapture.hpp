/*
 * InputCapture.hpp
 *
 *  Created on: 30 oct. 2022
 *      Author: alejandro 
 */

#pragma once
#include "ST-LIB-CORE.hpp"

#ifdef HAL_TIM_MODULE_ENABLED
class InputCapture {
public:
	class Instance {
	public:
		uint8_t id;
		Pin pin;
		TIM_HandleTypeDef* timer;
		uint32_t channel_rising;
		uint32_t channel_falling;
		uint32_t frequency;
		uint8_t duty_cycle;

		Instance() = default;
		Instance(Pin pin, TIM_HandleTypeDef* timer, uint32_t channel_rising, uint32_t channel_falling);
	};

	static map<uint8_t, InputCapture::Instance> instances;
	static map<Pin, InputCapture::Instance> instances_data;
	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin& pin);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static optional<uint32_t> read_frequency(uint8_t id);
	static optional<uint8_t> read_duty_cycle(uint8_t id);
	static Instance find_instance_by_channel(uint32_t channel);
};
#endif
