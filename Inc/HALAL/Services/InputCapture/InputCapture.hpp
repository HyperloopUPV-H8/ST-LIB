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
	struct data {
		uint32_t counter_values[4];
		uint8_t count;
		uint32_t frequency;
		uint8_t duty_cycle;
	};

	static map<uint32_t, uint32_t> channel_dict;
	static map<uint8_t, Pin> service_ids;
	static map<Pin, TimerChannelRisingFalling> pin_timer_map;
	static map<TimerChannelRisingFalling, IC::data> data_map;

	static forward_list<uint8_t> id_manager;
	static optional<uint8_t> register_ic(Pin& pin);
	static void unregister_ic(uint8_t id);
	static void turn_on_ic(uint8_t);
	static void turn_off_ic(uint8_t);
	static float read_frequency(uint8_t id);
	static uint8_t read_duty_cycle(uint8_t id);
	static ChannelsRisingFalling find_other_channel(uint32_t channel);
};
