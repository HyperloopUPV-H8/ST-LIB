/*
 * Time.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#include <Time/Time.hpp>1

uint64_t Time::global_tick = 0;
uint64_t Time::low_precision_tick = 0;

TIM_HandleTypeDef* Time::global_timer = &htim2;
set<TIM_HandleTypeDef*> Time::high_precision_timers;
TIM_HandleTypeDef* Time::low_precision_timer = &htim6;

stack<TIM_HandleTypeDef*> Time::available_high_precision_timers;

uint16_t Time::high_precision_alarm_id_count = 0;
uint16_t Time::low_precision_alarm_id_count = 0;
map<uint8_t, Time::Alarm> Time::high_precision_alarms_by_id;
map<uint8_t, Time::Alarm> Time::low_precision_alarms_by_id;
map<TIM_HandleTypeDef*, Time::Alarm> Time::high_precision_alarms_by_timer;

void Time::start(){
	Time::start_timer(global_timer, 0, Time::HIGH_PRECISION_MAX_ARR);
	Time::start_timer(low_precision_timer, 275, 1000);

	Time::available_high_precision_timers.push(&htim24);
	Time::available_high_precision_timers.push(&htim5);

	Time::high_precision_timers.insert(&htim24);
	Time::high_precision_timers.insert(&htim5);
}

void Time::start_timer(TIM_HandleTypeDef* handle, uint32_t prescaler, uint32_t arr){
	handle -> Instance -> ARR = arr;
	handle -> Instance -> PSC = prescaler;
	HAL_TIM_Base_Start_IT(handle);
}

void Time::stop_timer(TIM_HandleTypeDef* handle){
	HAL_TIM_Base_Stop_IT(handle);
}

// PUBLIC SERVICE METHODS

uint64_t Time::get_global_tick(){
	uint64_t current_tick = Time::global_tick + global_timer->Instance->CNT;
	double to_nanoseconds = 1.0 / Time::APB1_TIM_FREQ * 1000000000.0;
	return current_tick * to_nanoseconds;
}


optional<uint8_t> Time::register_high_precision_alarm(uint32_t period_in_us, function<void()> func){
	if(available_high_precision_timers.size() == 0)
		return {};

	uint16_t id = Time::high_precision_alarm_id_count++;
	TIM_HandleTypeDef* tim = Time::available_high_precision_timers.top();
	Time::available_high_precision_timers.pop();

	Time::Alarm alarm = { period_in_us, tim, func };
	Time::high_precision_alarms_by_id[id] = alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;

	Time::start_timer(tim, 275, period_in_us);

	return {id};
}

bool Time::unregister_high_precision_alarm(uint16_t id){
	if(Time::high_precision_alarms_by_id.find(id) == high_precision_alarms_by_id.end())
		return false;

	Time::Alarm alarm = high_precision_alarms_by_id[id];
	Time::available_high_precision_timers.push(alarm.tim);
	Time::stop_timer(alarm.tim);

	Time::high_precision_alarms_by_id.erase(id);
	Time::high_precision_alarms_by_timer.erase(alarm.tim);

	return true;
}

uint8_t Time::register_low_precision_alarm(uint32_t period_in_us, function<void()> func){
	uint16_t id = Time::low_precision_alarm_id_count++;
	Time::Alarm alarm = { period_in_us, low_precision_timer, func };
	Time::low_precision_alarms_by_id[id] = alarm;
	return { id };
}

bool Time::unregister_low_precision_alarm(uint16_t id){
	if(Time::low_precision_alarms_by_id.find(id) == low_precision_alarms_by_id.end())
		return false;
	Time::low_precision_alarms_by_id.erase(id);
	return true;
}

void Time::set_timeout(int millseconds, function<void()> callback){
	int id = Time::register_low_precision_alarm(millseconds, [&](){
		callback();
		Time::unregister_low_precision_alarm(id);
	});
}

void Time::global_timer_callback(){
	Time::global_tick += Time::HIGH_PRECISION_MAX_ARR;
}

void Time::high_precision_timer_callback(TIM_HandleTypeDef* tim){
	Time::high_precision_alarms_by_timer[tim].alarm();
}

void Time::low_precision_timer_callback(){
	for(auto pair : Time::low_precision_alarms_by_id){
		Time::Alarm alarm = pair.second;
		if(Time::low_precision_tick % alarm.period == 0)
			alarm.alarm();
	}

	low_precision_tick += 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim){
	if(tim == Time::global_timer)
			Time::global_timer_callback();
	if(Time::high_precision_timers.count(tim) !=0)
			Time::high_precision_timer_callback(tim);
	if(tim == Time::low_precision_timer)
			Time::low_precision_timer_callback();
}
