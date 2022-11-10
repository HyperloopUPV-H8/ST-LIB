/*
 * InputCapture.hpp
 *
 *  Created on: 30 oct. 2022
 *      Author: aleja
 */

#pragma once
#include "ST-LIB.hpp"
#include "TimerChannelModel/TimerChannel.hpp"

#define IC_OVERFLOW 0xffffffff

class IC {
public:
	class Instance {
	public:
		Pin pin;
		TIM_HandleTypeDef* timer;
		uint32_t channel_rising;
		uint32_t channel_falling;
		uint32_t counter_values[4];
		uint8_t count;
		uint32_t frequency;
		uint8_t duty_cycle;

		Instance() = default;
		Instance(Pin p, TIM_HandleTypeDef* tim, uint32_t ch_r, uint32_t ch_f);
	};

	struct instance_constructor_data {
		TIM_HandleTypeDef* timer;
		uint32_t channel_rising;
		uint32_t channel_falling;
	};

	static map<uint8_t, IC::Instance> instances;
	static map<Pin, instance_constructor_data> instances_data;
	static map<uint32_t, uint32_t> channel_dict;
	static forward_list<uint8_t> id_manager;
	static optional<uint8_t> register_ic(Pin& pin);
	static void unregister_ic(uint8_t id);
	static void turn_on_ic(uint8_t);
	static void turn_off_ic(uint8_t);
	static float read_frequency(uint8_t id);
	static uint8_t read_duty_cycle(uint8_t id);
	static Instance find_instance_by_channel(uint32_t channel);
};
