/*
 * PhasedPWM.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include "PWM/PhasedPWM/PhasedPWM.hpp"

PhasedPWM::PhasedPWM(Pin& pin) {
	if (not TimerPeripheral::available_pwm.contains(pin)) {
		ErrorHandler("Pin %s is not registered as an available PWM", pin.to_string());
		return;
	}

	TimerPeripheral& timer = TimerPeripheral::available_pwm.at(pin).first;
	TimerPeripheral::PWMData& pwm_data = TimerPeripheral::available_pwm.at(pin).second;

	if (pwm_data.mode != TimerPeripheral::PWM_MODE::PHASED) {
		ErrorHandler("Pin %s is not registered as a PHASED PWM", pin.to_string());
	}

	peripheral = &timer;
	channel = pwm_data.channel;

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	timer.init_data.pwm_channels.push_back(pwm_data);

	duty_cycle = 0;
	phase = 0;
}

void PhasedPWM::set_duty_cycle(float duty_cycle) {
	this->duty_cycle = duty_cycle;
	uint32_t arr = peripheral->handle->Instance->ARR;
	float raw_duty = arr - (arr / 100.0 * duty_cycle);
	float raw_phase = raw_duty * phase / 100.0;

	__HAL_TIM_SET_COMPARE(peripheral->handle, channel, raw_duty + raw_phase);

	if (channel % 8 == 0) {
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel + 4, raw_duty - raw_phase);
	} else {
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel - 4, raw_duty - raw_phase);
	}
}

void PhasedPWM::set_frequency(uint32_t frequency) {
	TIM_TypeDef& timer = *peripheral->handle->Instance;
	timer.ARR = (HAL_RCC_GetPCLK1Freq()*2 / (timer.PSC+1)) / 2 / frequency;
	set_duty_cycle(duty_cycle);
}

void PhasedPWM::set_phase(float phase) {
	this->phase = phase;
	set_duty_cycle(duty_cycle);
}
