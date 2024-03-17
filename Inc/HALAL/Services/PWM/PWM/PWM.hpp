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
protected:
	TimerPeripheral* peripheral;
	uint32_t channel;
	float duty_cycle;
	uint32_t frequency;
	bool is_on = false;


public:
	PWM() = default;
	PWM(Pin& pin);

	void turn_on();
	void turn_off();
	void set_duty_cycle(float duty_cycle);
	void set_frequency(uint32_t frequency);
	uint32_t get_frequency();
	float get_duty_cycle();
	void set_dead_time(std::chrono::nanoseconds dead_time_ns);
};

#endif
