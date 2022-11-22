/*
 * PWM_Module.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#pragma once
#include "ST-LIB-CORE.hpp"

#ifdef HAL_TIM_MODULE_ENABLED
class PWM {
public:
	static map<uint8_t, Pin> service_ids;
	static map<uint8_t, Pin> service_ids_negated;
	static map<uint8_t, pair<Pin, Pin>> service_ids_dual;
	static map<Pin, TimerChannel> pin_timer_map;
	static map<Pin, TimerChannel> pin_timer_map_negated;
	static map<pair<Pin, Pin>, TimerChannel> pin_timer_map_dual;

	static forward_list<uint8_t> id_manager;

	static optional<uint8_t> inscribe(Pin& pin);
	static optional<uint8_t> inscribe_negated(Pin& pin);
	static optional<uint8_t> inscribe_dual(Pin& pin, Pin& pin_negated);
	static void turn_on(uint8_t id);
	static void turn_off(uint8_t id);
	static void set_duty(uint8_t id, uint8_t duty_cicle);
};
#endif
