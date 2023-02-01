/*
 * Time.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#include <Time/Time.hpp>

TIM_HandleTypeDef* Time::global_timer = &htim2;
TIM_HandleTypeDef* Time::low_precision_timer = &htim6;

uint8_t Time::high_precision_ids = 0;
uint8_t Time::low_precision_ids = 0;

stack<TIM_HandleTypeDef*> Time::available_high_precision_timers;
set<TIM_HandleTypeDef*> Time::high_precision_timers;

map<uint8_t, Time::Alarm> Time::high_precision_alarms_by_id;
map<uint8_t, Time::Alarm> Time::low_precision_alarms_by_id;
map<TIM_HandleTypeDef*, Time::Alarm> Time::high_precision_alarms_by_timer;

uint64_t Time::global_tick = 0;
uint64_t Time::low_precision_tick = 0;

void Time::init_timer(TIM_TypeDef* tim, TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period){
	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  htim->Instance = tim;
	  htim->Init.Prescaler = prescaler;
	  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim->Init.Period = period;
	  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  HAL_TIM_Base_Init(htim);

	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig);

	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	  HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);

}

void Time::start(){
	Time::init_timer(TIM2, &htim2, 0, HIGH_PRECISION_MAX_ARR);
	Time::init_timer(TIM5, &htim5, 0, HIGH_PRECISION_MAX_ARR);
	Time::init_timer(TIM24, &htim24, 0, HIGH_PRECISION_MAX_ARR);
	Time::init_timer(TIM6, &htim6, 275, 1000);

	HAL_TIM_Base_Start_IT(global_timer);
	HAL_TIM_Base_Start_IT(low_precision_timer);

	Time::available_high_precision_timers.push(&htim24);
	Time::available_high_precision_timers.push(&htim5);

	Time::high_precision_timers.insert(&htim24);
	Time::high_precision_timers.insert(&htim5);
}


// PUBLIC SERVICE METHODS

uint64_t Time::get_global_tick(){
	uint64_t current_tick = Time::global_tick + global_timer->Instance->CNT;
	uint32_t apb1_tim_freq = HAL_RCC_GetPCLK1Freq()*2;
	double to_nanoseconds = 1.0 / apb1_tim_freq * 1000000000.0;
	return current_tick * to_nanoseconds;
}

void Time::start_timer(TIM_HandleTypeDef* handle, uint32_t prescaler, uint32_t arr){
	handle -> Instance -> ARR = arr;
	handle -> Instance -> PSC = prescaler;
	HAL_TIM_Base_Start_IT(handle);
}

void Time::stop_timer(TIM_HandleTypeDef* handle){
	HAL_TIM_Base_Stop_IT(handle);
}

optional<uint8_t> Time::register_high_precision_alarm(uint32_t period_in_us, function<void()> func){
	if(available_high_precision_timers.size() == 0) {
		return nullopt;
	}

	TIM_HandleTypeDef* tim = Time::available_high_precision_timers.top();
	Time::available_high_precision_timers.pop();

	Time::Alarm alarm = {
			.period = period_in_us,
			.tim = tim,
			.alarm = func
	};
	Time::high_precision_alarms_by_id[high_precision_ids]= alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;

	tim->Instance->PSC = 275;
	tim->Instance->ARR = period_in_us;
	HAL_TIM_Base_Start_IT(tim);

	return high_precision_ids++;
}

bool Time::unregister_high_precision_alarm(uint16_t id){
	if(not Time::high_precision_alarms_by_id.contains(id)) {
		return false;
	}

	Time::Alarm alarm = high_precision_alarms_by_id[id];
	Time::available_high_precision_timers.push(alarm.tim);
	Time::stop_timer(alarm.tim);

	Time::high_precision_alarms_by_id.erase(id);
	Time::high_precision_alarms_by_timer.erase(alarm.tim);

	return true;
}

uint8_t Time::register_low_precision_alarm(uint32_t period_in_ms, function<void()> func){
	Time::Alarm alarm = {
			.period = period_in_ms,
			.tim = low_precision_timer,
			.alarm = func
	};
	Time::low_precision_alarms_by_id[low_precision_ids] = alarm;

	return low_precision_ids++;
}

bool Time::unregister_low_precision_alarm(uint16_t id){
	if(not Time::low_precision_alarms_by_id.contains(id)){
		return false;
	}
	Time::low_precision_alarms_by_id.erase(id);

	return true;
}

void Time::set_timeout(int milliseconds, function<void()> callback){
	uint8_t id = low_precision_ids;
	Time::register_low_precision_alarm(milliseconds, [&,id](){
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
	if(tim == Time::global_timer) {
			Time::global_timer_callback();
	}
	else if(tim == Time::low_precision_timer) {
			Time::low_precision_timer_callback();
	}

	else if(Time::high_precision_timers.contains(tim)) {
			Time::high_precision_timer_callback(tim);
	}
}
