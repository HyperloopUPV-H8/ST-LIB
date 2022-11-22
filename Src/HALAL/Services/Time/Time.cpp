/*
 * Time.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#include <Time/Time.hpp>

TIM_HandleTypeDef* Time::global_timer = &htim2;
TIM_HandleTypeDef* Time::low_precision_timer = &htim6;

forward_list<uint8_t> Time::high_precision_ids = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
forward_list<uint8_t> Time::low_precision_ids = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
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

	uint16_t id = Time::high_precision_ids.front();
	Time::high_precision_ids.pop_front();

	TIM_HandleTypeDef* tim = Time::available_high_precision_timers.top();
	Time::available_high_precision_timers.pop();

	Time::Alarm alarm = {
			.period = period_in_us,
			.tim = tim,
			.alarm = func
	};
	Time::high_precision_alarms_by_id[id] = alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;

	tim->Instance->PSC = 275;
	tim->Instance->ARR = period_in_us;
	HAL_TIM_Base_Start_IT(tim);

	return id;
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

optional<uint8_t> Time::register_low_precision_alarm(uint32_t period_in_ms, function<void()> func){
	uint16_t id = Time::low_precision_ids.front();
	Time::low_precision_ids.pop_front();

	Time::Alarm alarm = { period_in_ms, low_precision_timer, func };
	Time::low_precision_alarms_by_id[id] = alarm;
	Time::high_precision_ids.push_front(id);
	return id;
}

bool Time::unregister_low_precision_alarm(uint16_t id){
	if(not Time::low_precision_alarms_by_id.contains(id)){
		return false;
	}
	Time::low_precision_alarms_by_id.erase(id);
	Time::high_precision_ids.push_front(id);
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
