/*
 * LowPowerTimer.hpp
 *
 *  Created on: Nov 23, 2022
 *      Author: alejandro
 */

#pragma once

struct LowPowerTimer {
public:
	LPTIM_HandleTypeDef* handle;
	uint16_t period;

	LowPowerTimer() = default;
	LowPowerTimer(LPTIM_HandleTypeDef* handle, uint16_t period) : handle(handle), period(period) {};
};

