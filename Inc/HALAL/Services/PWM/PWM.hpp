/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#pragma once

#include "../../Models/PinModel/Pin.hpp"
#include "../../../C++Utilities/CppUtils.hpp"


struct TimerChannel {
	TIM_HandleTypeDef* timer;
	unsigned int channel;

	//TimerChannel() = default;
	//TimerChannel(uint32_t t, unsigned int c) :timer(t),channel(c) {}
};

class PWM {
public:
	static map<uint8_t, Pin> serviceIDs;
	static map<Pin, TimerChannel> pinTimerMap;

	//static unordered_map<Pin, TimerChannel, KeyHash> pinTimerMap;
	static forward_list<uint8_t> IDmanager;

	static optional<uint8_t> register_pwm(Pin& pin);
	static void unregister_pwm(uint8_t id);
	static void turn_on_pwm(uint8_t id);
	static void turn_off_pwm(uint8_t id);
	static void change_duty_cycle(uint8_t id, uint8_t duty_cicle);
};