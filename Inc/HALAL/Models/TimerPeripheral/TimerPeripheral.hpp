/*
 * TimerPeripheral.hpp
 *
 *  Created on: 3 dic. 2022
 *      Author: aleja
 */

#pragma once
#include "stm32h7xx_hal.h"
#include "C++Utilities/CppUtils.hpp"

class TimerPeripheral {
public:
	struct InitData {
		TIM_TypeDef* timer;
		uint32_t prescaler;
		uint32_t period;
		uint32_t deadtime;
		vector<uint32_t> channels;
		InitData() = default;
		InitData(TIM_TypeDef* timer, uint32_t prescaler = 275,
				uint32_t period = 1000, uint32_t deadtime = 0);
	};

	TIM_HandleTypeDef* handle;
	InitData init_data;

	TimerPeripheral() = default;
	TimerPeripheral(TIM_HandleTypeDef* handle, InitData init_data);

	bool is_registered();

};
