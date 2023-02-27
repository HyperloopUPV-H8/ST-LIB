/*
 * PWMInstance.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "TimerPeripheral/TimerPeripheral.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

class PWMInstance {
private:
	TimerPeripheral* peripheral;
	uint32_t channel;

	void turn_on();
	void turn_off();
	void set_duty_cycle(uint8_t duty_cycle);

public:
	friend class PWMservice;
	friend class DualPWMInstance;
	friend class PhasedPWMInstance;
};

#endif
