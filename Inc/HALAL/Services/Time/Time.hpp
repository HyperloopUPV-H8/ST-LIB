/*
 * Time.hpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#pragma once

#include "stm32h7xx_hal.h"

#define RTC_MAX_COUNTER 32767

#ifdef HAL_TIM_MODULE_ENABLED

#include "C++Utilities/CppUtils.hpp"

// HIGH RESOLUTION TIMERS
extern TIM_HandleTypeDef htim2;		// Used for the global timer (3,36nS step)

extern TIM_HandleTypeDef htim5;		// Used for the high precision alarms (1uS)
extern TIM_HandleTypeDef htim24;	// Used for the high precision alarms (1uS)

extern TIM_HandleTypeDef htim23;    // Used for the mid precision alarms (50uS)

// LOW RESOLUTION TIMERS
extern TIM_HandleTypeDef htim7;		// Used for the low precision alarms (1mS)

class Time {

private :
	static uint8_t high_precision_ids;
	static uint8_t low_precision_ids;
	static uint8_t mid_precision_ids;

	struct Alarm {
		uint32_t period;
		TIM_HandleTypeDef* tim;
		function<void()> alarm;
		uint64_t offset;
	};

	static constexpr uint32_t HIGH_PRECISION_MAX_ARR = 4294967295;
	static constexpr uint32_t MID_PRECISION_MAX_ARR = 4294967295;
	static constexpr uint32_t mid_precision_step_in_us = 50;
	static uint64_t global_tick;
	static uint64_t low_precision_tick;
	static uint64_t mid_precision_tick;
	static bool mid_precision_registered;

	static unordered_map<uint8_t, Alarm> high_precision_alarms_by_id;
	static unordered_map<TIM_HandleTypeDef*, Alarm> high_precision_alarms_by_timer;
	static unordered_map<uint8_t, Alarm> low_precision_alarms_by_id;
	static unordered_map<uint8_t, Alarm> mid_precision_alarms_by_id;

	static void stop_timer(TIM_HandleTypeDef* htim);
	static void start_timer(TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period);
	static void init_timer(TIM_TypeDef* tim, TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period, IRQn_Type interrupt_channel);
	static void ConfigTimer(TIM_HandleTypeDef* tim, uint32_t period_in_us);

public :
	static TIM_HandleTypeDef* global_timer;

	static set<TIM_HandleTypeDef*> high_precision_timers;
	static stack<TIM_HandleTypeDef*> available_high_precision_timers;

	static TIM_HandleTypeDef* low_precision_timer;
	static TIM_HandleTypeDef* mid_precision_timer;

	static void high_precision_timer_callback(TIM_HandleTypeDef* tim);
	static void global_timer_callback();
	static void low_precision_timer_callback();
	static void mid_precision_timer_callback();

	/**
	 * @brief Initializes instances registered with init_timer.
	 *
	 * @return void
	 */
	static void start();

	static uint64_t get_global_tick();

	/**
	* @brief Tries to register a high_precision_alarm that will execute a function cyclically
	* until unregistered.
	*
	* @param period_in_us period in microseconds until timeout.
	* @param func function to be executed on timeout.
	* @return optional<uint8_t> Returns id of the alarm if succesful, nullopt if it is able
	* to register the timer correctly (probably because there aren't any timer available).
	*/
	static optional<uint8_t> register_high_precision_alarm(uint32_t period_in_us, function<void()> func);

	static bool unregister_high_precision_alarm(uint8_t id);

	/**
	* @brief Registers a low_precision_alarm that will execute a function cyclically until unregistered.
	*
	* @param period_in_ms period in milliseconds until timeout.
	* @param func function to be executed on timeout.
	* @return uint8_t Returns id of the alarm.
	*/
	static uint8_t register_low_precision_alarm(uint32_t period_in_ms, function<void()> func);
	static bool unregister_low_precision_alarm(uint8_t id);

	/**
	* @brief Registers a low_precision_alarm, executes an action ONLY ONE TIME
	* and unregisters the alarm.
	*
	* @param milliseconds time until the action is executed.
	* @param func function to be executed on timeout.
	* @return void
	*/

	static optional<uint8_t> register_mid_precision_alarm(uint32_t period_in_us, function<void()> func);
	static bool unregister_mid_precision_alarm(uint16_t id);

	static void set_timeout(int milliseconds, function<void()> callback);

#ifdef HAL_RTC_MODULE_ENABLED
	struct RTCData{
		uint16_t counter;
		uint8_t second;
		uint8_t minute;
		uint8_t hour;
		uint8_t day;
		uint8_t month;
		uint16_t year;

		string serialize() const {
			return "Counter: " + to_string(counter) + ",Second: " + to_string(second) + ",Minute: " + to_string(minute) + ",Hour: " + to_string(hour) + ",Day: " + to_string(day) + ",Month: " + to_string(month) + ",Year: " + to_string(year);
		}

		static size_t get_string_size(const RTCData& to_serialize){
			return to_serialize.serialize().size();
		}
	};

	static void start_rtc();
	static RTCData get_rtc_data();
	static void set_rtc_data(uint16_t counter, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year);

	static RTC_HandleTypeDef hrtc;
#endif
};

#endif
