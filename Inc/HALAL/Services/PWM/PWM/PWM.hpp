/*
 * PWM.hpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#pragma once

#include "PinModel/Pin.hpp"
#include "TimerPeripheral/TimerPeripheral.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

class PWM {
public:
	TimerPeripheral* peripheral;
	uint32_t channel;
	float duty_cycle;

	PWM() = default;

public:
	PWM(Pin& pin);

	void turn_on();
	void turn_off();
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);

	friend class DualPWM;
	friend class PhasedPWM;
	friend class DualPhasedPWM;
};

#endif
