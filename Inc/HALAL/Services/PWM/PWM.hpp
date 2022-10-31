/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#pragma once
#include "ST-LIB.hpp"


struct TimerChannel {
	TIM_HandleTypeDef* timer;
	unsigned int channel;
};

class PWM {
public:
	static map<uint8_t, Pin> service_ids;
	static map<Pin, TimerChannel> pinTimerMap;

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_pwm(Pin& pin);
	static void unregister_pwm(uint8_t id);
	static void turn_on_pwm(uint8_t id);
	static void turn_on_pwm_negated(uint8_t id);
	static void turn_off_pwm(uint8_t id);
	static void turn_off_pwm_negated(uint8_t id);
	static void change_duty_cycle(uint8_t id, uint8_t duty_cicle);
};
