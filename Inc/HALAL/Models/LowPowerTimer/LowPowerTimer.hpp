/*
 * LowPowerTimer.hpp
 *
 *  Created on: Nov 23, 2022
 *      Author: alejandro
 */

#pragma once

#include "stm32h7xx_hal.h"

#ifdef HAL_LPTIM_MODULE_ENABLED

class LowPowerTimer {
public:
	LPTIM_TypeDef& instance;
	LPTIM_HandleTypeDef& handle;
	uint16_t period;

	LowPowerTimer() = default;
	LowPowerTimer(LPTIM_TypeDef& instance, LPTIM_HandleTypeDef& handle, uint16_t period) :
		instance(instance), handle(handle), period(period) {};

	void init();
};

#endif
