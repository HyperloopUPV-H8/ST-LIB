/*
 * Time.hpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#pragma once
#include "C++Utilities/CppUtils.hpp"
#include "stm32h7xx_hal.h"

// HIGH RESOLUTION TIMERS
extern TIM_HandleTypeDef htim2;		// Used for the global timer (3,36nS step)

extern TIM_HandleTypeDef htim5;		// Used for the high precision alarms (1uS)
extern TIM_HandleTypeDef htim24;	// Used for the high precision alarms (1uS)

// LOW RESOLUTION TIMERS
extern TIM_HandleTypeDef htim6;		// Used for the low precision alarms (1mS)

class Time {

private :

	static forward_list<uint8_t> high_precision_ids;
	static forward_list<uint8_t> low_precision_ids;

	struct Alarm {
		uint32_t period;
		TIM_HandleTypeDef* tim;
		function<void()> alarm;
	};

	static const uint32_t HIGH_PRECISION_MAX_ARR = 4294967295;
	static uint64_t global_tick;
	static uint64_t low_precision_tick;

	static map<uint8_t, Alarm> high_precision_alarms_by_id;
	static map<TIM_HandleTypeDef*, Alarm> high_precision_alarms_by_timer;
	static map<uint8_t, Alarm> low_precision_alarms_by_id;

	static void stop_timer(TIM_HandleTypeDef* htim);
	static void start_timer(TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period);
	static void init_timer(TIM_TypeDef* tim, TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period);

public :
	static TIM_HandleTypeDef* global_timer;

	static set<TIM_HandleTypeDef*> high_precision_timers;
	static stack<TIM_HandleTypeDef*> available_high_precision_timers;
	static bool is_low_precision_timer_registered;

	static TIM_HandleTypeDef* low_precision_timer;

	static void high_precision_timer_callback(TIM_HandleTypeDef* tim);
	static void global_timer_callback();
	static void low_precision_timer_callback();

	static void start();
	static uint64_t get_global_tick();
	static optional<uint8_t> register_high_precision_alarm(uint32_t period_in_us, function<void()> func);
	static bool unregister_high_precision_alarm(uint16_t id);

	static optional<uint8_t> register_low_precision_alarm(uint32_t period_in_ms, function<void()> func);
	static bool unregister_low_precision_alarm(uint16_t id);

	static void set_timeout(int milliseconds, function<void()> callback);
};
