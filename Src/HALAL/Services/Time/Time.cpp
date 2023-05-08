/*
 * Time.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#include <Time/Time.hpp>
#include "ErrorHandler/ErrorHandler.hpp"
#include "TimerPeripheral/TimerPeripheral.hpp"


RTC_HandleTypeDef Time::hrtc;

TIM_HandleTypeDef* Time::global_timer = &htim2;
TIM_HandleTypeDef* Time::low_precision_timer = &htim7;
TIM_HandleTypeDef* Time::mid_precision_timer = &htim23;

uint8_t Time::high_precision_ids = 0;
uint8_t Time::low_precision_ids = 0;
uint8_t Time::mid_precision_ids = 0;

bool Time::mid_precision_registered = false;

stack<TIM_HandleTypeDef*> Time::available_high_precision_timers;
set<TIM_HandleTypeDef*> Time::high_precision_timers;

unordered_map<uint8_t, Time::Alarm> Time::high_precision_alarms_by_id;
unordered_map<uint8_t, Time::Alarm> Time::low_precision_alarms_by_id;
unordered_map<uint8_t, Time::Alarm> Time::mid_precision_alarms_by_id;
unordered_map<TIM_HandleTypeDef*, Time::Alarm> Time::high_precision_alarms_by_timer;

uint64_t Time::global_tick = 0;
uint64_t Time::low_precision_tick = 0;
uint64_t Time::mid_precision_tick = 0;

void Time::init_timer(TIM_TypeDef* tim, TIM_HandleTypeDef* htim,uint32_t prescaler, uint32_t period, IRQn_Type interrupt_channel){
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

	  HAL_NVIC_SetPriority(interrupt_channel, 0, 0);
	  HAL_NVIC_EnableIRQ(interrupt_channel);

}

void Time::start(){
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM5_CLK_ENABLE();
    __HAL_RCC_TIM7_CLK_ENABLE();
    __HAL_RCC_TIM24_CLK_ENABLE();

	Time::init_timer(TIM2, &htim2, 0, HIGH_PRECISION_MAX_ARR, TIM2_IRQn);
	Time::init_timer(TIM5, &htim5, 0, HIGH_PRECISION_MAX_ARR, TIM5_IRQn);
	Time::init_timer(TIM24, &htim24, 0, HIGH_PRECISION_MAX_ARR, TIM24_IRQn);
	Time::init_timer(TIM7, &htim7, 275, 1000, TIM7_IRQn);

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
		ErrorHandler("There are no available high precision timers left");
		return nullopt;
	}

	TIM_HandleTypeDef* tim = Time::available_high_precision_timers.top();
	Time::available_high_precision_timers.pop();

	Time::Alarm alarm = {
			.period = period_in_us,
			.tim = tim,
			.alarm = [&](){},
	};

	NVIC_DisableIRQ(TIM5_IRQn);
	NVIC_DisableIRQ(TIM24_IRQn);
	Time::high_precision_alarms_by_id[high_precision_ids]= alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;


	Time::ConfigTimer(tim, period_in_us);

	alarm.alarm = func;
	Time::high_precision_alarms_by_id[high_precision_ids] = alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_EnableIRQ(TIM24_IRQn);

	return high_precision_ids++;
}


bool Time::unregister_high_precision_alarm(uint8_t id){
	if(not Time::high_precision_alarms_by_id.contains(id)) {
		return false;
	}

	NVIC_DisableIRQ(TIM5_IRQn);
	NVIC_DisableIRQ(TIM24_IRQn);
	Time::Alarm alarm = high_precision_alarms_by_id[id];
	Time::available_high_precision_timers.push(alarm.tim);
	Time::stop_timer(alarm.tim);

	Time::high_precision_alarms_by_id.erase(id);
	Time::high_precision_alarms_by_timer.erase(alarm.tim);
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_EnableIRQ(TIM24_IRQn);

	return true;
}

optional<uint8_t> Time::register_mid_precision_alarm(uint32_t period_in_us, function<void()> func){
	if(std::find_if(TimerPeripheral::timers.begin(),TimerPeripheral::timers.end(), [&](TimerPeripheral& tim) -> bool {
		return tim.handle == &htim23 && (tim.is_occupied());}) !=  TimerPeripheral::timers.end()){
		ErrorHandler("htim 23 cannot be used as mid precision timer and PWM or Input Capture Simultaneously");
		return nullopt;
	}

	Time::Alarm alarm = {
			.period = period_in_us/Time::mid_precision_step_in_us,
			.tim = Time::mid_precision_timer,
			.alarm = [&](){},
			.offset = Time::mid_precision_tick
	};

	NVIC_DisableIRQ(TIM23_IRQn);
	Time::mid_precision_alarms_by_id[mid_precision_ids] = alarm;

	if(not Time::mid_precision_registered){
	    __HAL_RCC_TIM23_CLK_ENABLE();
		Time::init_timer(TIM23, Time::mid_precision_timer, 275, Time::mid_precision_step_in_us, TIM23_IRQn);
		Time::ConfigTimer(Time::mid_precision_timer, Time::mid_precision_step_in_us);
		NVIC_DisableIRQ(TIM23_IRQn);
		Time::mid_precision_registered = true;
	}

	alarm.alarm = func;
	Time::mid_precision_alarms_by_id[mid_precision_ids] = alarm;
	NVIC_EnableIRQ(TIM23_IRQn);

	return mid_precision_ids++;
}

bool Time::unregister_mid_precision_alarm(uint8_t id){
	if(not Time::mid_precision_alarms_by_id.contains(id)) {
		return false;
	}

	NVIC_DisableIRQ(TIM23_IRQn);
	Time::mid_precision_alarms_by_id.erase(id);
	NVIC_EnableIRQ(TIM23_IRQn);

	return true;
}

uint8_t Time::register_low_precision_alarm(uint32_t period_in_ms, function<void()> func){
	Time::Alarm alarm = {
			.period = period_in_ms,
			.tim = low_precision_timer,
			.alarm = func,
			.offset = low_precision_tick % period_in_ms
	};

	NVIC_DisableIRQ(TIM7_IRQn);
	Time::low_precision_alarms_by_id[low_precision_ids] = alarm;
	NVIC_EnableIRQ(TIM7_IRQn);
	return low_precision_ids++;
}

uint8_t Time::register_low_precision_alarm(uint32_t period_in_ms, void(*func)()){
	Time::Alarm alarm = {
			.period = period_in_ms,
			.tim = low_precision_timer,
			.alarm = func,
			.offset = low_precision_tick % period_in_ms
	};

	NVIC_DisableIRQ(TIM7_IRQn);
	Time::low_precision_alarms_by_id[low_precision_ids] = alarm;
	NVIC_EnableIRQ(TIM7_IRQn);
	return low_precision_ids++;
}

bool Time::unregister_low_precision_alarm(uint8_t id){
	if(not Time::low_precision_alarms_by_id.contains(id)){
		return false;
	}

	NVIC_DisableIRQ(TIM7_IRQn);
	Time::low_precision_alarms_by_id.erase(id);
	NVIC_EnableIRQ(TIM7_IRQn);

	return true;
}

void Time::set_timeout(int milliseconds, function<void()> callback){
	uint8_t id = low_precision_ids;
	uint64_t tick_on_register = low_precision_tick;
	Time::register_low_precision_alarm(milliseconds, [&,id,callback,tick_on_register](){
		if(tick_on_register == low_precision_tick){
			return;
		}
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

void Time::mid_precision_timer_callback(){
	for(pair<const uint8_t,Time::Alarm>& pair : Time::mid_precision_alarms_by_id){
		Time::Alarm& alarm = pair.second;
		if((Time::mid_precision_tick - alarm.offset) % alarm.period == 0){
			alarm.alarm();
		}
	}
	Time::mid_precision_tick++;
}

void Time::low_precision_timer_callback(){
	for(auto& pair : Time::low_precision_alarms_by_id){
		Time::Alarm& alarm = pair.second;
		if((Time::low_precision_tick - alarm.offset) % alarm.period == 0){
			alarm.alarm();
		}
	}
	low_precision_tick += 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* tim){
	if(tim == Time::global_timer) {
		Time::global_timer_callback();
	}else if(tim == Time::mid_precision_timer){
		Time::mid_precision_timer_callback();
	}
	else if(tim == Time::low_precision_timer) {
		Time::low_precision_timer_callback();
	}

	else if(Time::high_precision_timers.contains(tim)) {
		Time::high_precision_timer_callback(tim);
	}
}

void Time::ConfigTimer(TIM_HandleTypeDef* tim, uint32_t period_in_us){
	__HAL_TIM_DISABLE_IT(tim,TIM_IT_UPDATE);
	tim->Instance->CR1 = ~(TIM_CR1_DIR|TIM_CR1_CMS) & tim->Instance->CR1;
	tim->Instance->CR1 = TIM_COUNTERMODE_UP | tim->Instance->CR1;
	tim->Instance->CR1 = ~TIM_CR1_CKD & tim->Instance->CR1;
	tim->Instance->CR1 = TIM_CLOCKDIVISION_DIV1 | tim->Instance->CR1;
	tim->Instance->ARR = period_in_us;
	tim->Instance->PSC = 275;
	tim->Instance->EGR = TIM_EGR_UG;
	tim->Instance->SMCR = 0;
	tim->Instance->BDTR = 0;
	tim->Instance->CR1 = TIM_CR1_CEN;
	tim->Instance->CNT = 1;
	tim->Instance->DIER = TIM_IT_UPDATE;

}

void Time::start_rtc(){
	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 0;
	hrtc.Init.SynchPrediv = 32767;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;

	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		ErrorHandler("Error on RTC Init");
	}
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		ErrorHandler("Error while setting time at RTC start");
	}

	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 23;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		ErrorHandler("Error while setting date at RTC start");
	}
}


Time::RTCData Time::get_rtc_data(){
	RTCData ret;
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
	ret.counter = gTime.SecondFraction - gTime.SubSeconds;
	ret.second = gTime.Seconds;
	ret.minute = gTime.Minutes;
	ret.hour = gTime.Hours;
	ret.day = gDate.Date;
	ret.month = gDate.Month;
	ret.year = 2000 + gDate.Year;
	return ret;
}

void Time::set_rtc_data(uint16_t counter, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year){
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;
	gTime.SubSeconds = counter;
	gTime.Seconds = second;
	gTime.Minutes = minute;
	gTime.Hours = hour;
	gTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	gTime.StoreOperation = RTC_STOREOPERATION_RESET;
	gDate.WeekDay = 0;
	gDate.Date = day;
	gDate.Month = month;
	gDate.Year = year - 2000;
	if (HAL_RTC_SetTime(&hrtc, &gTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		ErrorHandler("Error on writing Time on the RTC");
	}
	if (HAL_RTC_SetDate(&hrtc, &gDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		ErrorHandler("Error on writing Date on the RTC");
	}
}
