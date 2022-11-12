/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#pragma once
#include "ST-LIB.hpp"

class PWM {
public:
	static map<uint8_t, Pin> service_ids;
	static map<uint8_t, Pin> service_ids_negated;
	static map<uint8_t, pair<Pin, Pin>> service_ids_dual;
	static map<Pin, TimerChannel> pin_timer_map;
	static map<Pin, TimerChannel> pin_timer_map_negated;
	static map<pair<Pin, Pin>, TimerChannel> pin_timer_map_dual;

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> register_pwm(Pin& pin);
	static optional<uint8_t> register_pwm_negated(Pin& pin);
	static optional<uint8_t> register_pwm_dual(Pin& pin, Pin& pin_negated);

	static void unregister_pwm(uint8_t id);

	static void turn_on_pwm(uint8_t id);

	static void turn_off_pwm(uint8_t id);

	static void change_duty_cycle(uint8_t id, uint8_t duty_cicle);
};
