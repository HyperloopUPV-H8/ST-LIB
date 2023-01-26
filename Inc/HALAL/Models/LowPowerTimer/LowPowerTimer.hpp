/*
 * LowPowerTimer.hpp
 *
 *  Created on: Nov 23, 2022
 *      Author: alejandro
 */

#pragma once
#include <map>

#include "stm32h7xx_hal.h"

#ifdef HAL_LPTIM_MODULE_ENABLED

using std::reference_wrapper;
using std::map;

class LowPowerTimer {
public:
	LPTIM_TypeDef& instance;
	LPTIM_HandleTypeDef& handle;
	uint16_t period;

	LowPowerTimer() = default;
	LowPowerTimer(LPTIM_TypeDef& instance, LPTIM_HandleTypeDef& handle, uint16_t period) :
		instance(instance), handle(handle), period(period) {};

	const char* to_string() const;
	static const map<reference_wrapper<LPTIM_TypeDef>, const char*> lptim_to_string;

	void init();
};

#endif
