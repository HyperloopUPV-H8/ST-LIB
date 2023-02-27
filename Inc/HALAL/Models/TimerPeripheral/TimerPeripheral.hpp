/*
 * TimerPeripheral.hpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#pragma once
#include "stm32h7xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

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
	static constexpr map<TIM_HandleTypeDef*, TIM_TypeDef*> handle_to_timer;
};

#endif
