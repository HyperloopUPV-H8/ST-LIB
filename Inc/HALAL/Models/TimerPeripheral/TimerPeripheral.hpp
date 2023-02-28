/*
 * TimerPeripheral.hpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#pragma once
#include "stm32h7xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim18;
extern TIM_HandleTypeDef htim19;
extern TIM_HandleTypeDef htim20;
extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim22;
extern TIM_HandleTypeDef htim23;
extern TIM_HandleTypeDef htim24;

#include "C++Utilities/CppUtils.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

class TimerPeripheral {
public:
	struct InitData {
		TIM_TypeDef* timer;
		uint32_t prescaler;
		uint32_t period;
		uint32_t deadtime;
		bool is_base;
		vector<uint32_t> pwm_channels = {};
		vector<pair<uint32_t, uint32_t>> input_capture_channels = {};
		InitData() = default;
		InitData(bool is_base = false, uint32_t prescaler = 5,
				uint32_t period = 55000, uint32_t deadtime = 0);
	};

	TIM_HandleTypeDef* handle;
	InitData init_data;
	string name;
	static vector<reference_wrapper<TimerPeripheral>> timers;

	TimerPeripheral() = default;
	TimerPeripheral(TIM_HandleTypeDef* handle, InitData& init_data, string name);

	static void start();

	void init();
	bool is_registered();
	uint32_t get_prescaler();
	uint32_t get_period();

private:
	static map<TIM_HandleTypeDef*, TIM_TypeDef*> handle_to_timer;
};

#endif
