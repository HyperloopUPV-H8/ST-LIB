/*
 * Time.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: Dani
 */

#include <Time/Time.hpp>
#include "ErrorHandler/ErrorHandler.hpp"

RTC_HandleTypeDef Time::hrtc;

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
    __HAL_RCC_TIM6_CLK_ENABLE();
    __HAL_RCC_TIM24_CLK_ENABLE();

	Time::init_timer(TIM2, &htim2, 0, HIGH_PRECISION_MAX_ARR, TIM2_IRQn);
	Time::init_timer(TIM5, &htim5, 0, HIGH_PRECISION_MAX_ARR, TIM5_IRQn);
	Time::init_timer(TIM24, &htim24, 0, HIGH_PRECISION_MAX_ARR, TIM24_IRQn);
	Time::init_timer(TIM6, &htim6, 275, 1000, TIM6_DAC_IRQn);

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
			.alarm = [&](){}
	};
	Time::high_precision_alarms_by_id[high_precision_ids]= alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;


	Time::ConfigTimer(tim, period_in_us);

	alarm.alarm = func;
	Time::high_precision_alarms_by_id[high_precision_ids]= alarm;
	Time::high_precision_alarms_by_timer[tim] = alarm;

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

void Time::ConfigTimer(TIM_HandleTypeDef* tim, uint32_t period_in_us){
	__HAL_TIM_DISABLE_IT(tim,TIM_IT_UPDATE);
	tim->Instance->CR1 &= ~(TIM_CR1_DIR|TIM_CR1_CMS);
	tim->Instance->CR1 |= TIM_COUNTERMODE_UP;
	tim->Instance->CR1 &= ~TIM_CR1_CKD;
	tim->Instance->CR1 |= TIM_CLOCKDIVISION_DIV1;
	tim->Instance->ARR = period_in_us;
	tim->Instance->PSC = 275;
	tim->Instance->EGR = TIM_EGR_UG;
	tim->Instance->SMCR = 0;
	tim->Instance->BDTR = 0;
	tim->Instance->CR1 = TIM_CR1_CEN;
	tim->Instance->CNT = 1;
	tim->Instance->DIER = TIM_IT_UPDATE;

}

void Time::start_RTC(){


	RTC_TimeTypeDef sTime = {0};
		  RTC_DateTypeDef sDate = {0};

		  /* USER CODE BEGIN RTC_Init 1 */

		  /* USER CODE END RTC_Init 1 */

		  /** Initialize RTC Only
		  */
		  hrtc.Instance = RTC;
		  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
		  hrtc.Init.AsynchPrediv = 0;
		  hrtc.Init.SynchPrediv = 32767;
		  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
		  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
		  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
		  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
		  if (HAL_RTC_Init(&hrtc) != HAL_OK)
		  {
			  ErrorHandler("Error on RTC Init");
		  }

		  /* USER CODE BEGIN Check_RTC_BKUP */

		  /* USER CODE END Check_RTC_BKUP */

		  /** Initialize RTC and set the Time and Date
		  */
		  sTime.Hours = 0x0;
		  sTime.Minutes = 0x0;
		  sTime.Seconds = 0x0;
		  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		  {
			  ErrorHandler("Error while setting time at RTC start");
		  }
		  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		  sDate.Month = RTC_MONTH_JANUARY;
		  sDate.Date = 0x1;
		  sDate.Year = 0x0;

		  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		  {
			  ErrorHandler("Error while setting date at RTC start");
		  }
		  /* USER CODE BEGIN RTC_Init 2 */

		  /* USER CODE END RTC_Init 2 */
}


Time::rtc_data Time::get_RTC_data(){
	rtc_data ret;
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

void Time::set_RTC_data(uint16_t counter, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year){
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;
	gTime.SubSeconds = counter;
	gTime.Seconds = second;
	gTime.Minutes = minute;
	gTime.Hours = hour;
	gTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	gTime.StoreOperation = RTC_STOREOPERATION_RESET;
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
