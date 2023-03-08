/*
 * PhasedPWM.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include <PWM/PhasedPWM/PhasedPWM.hpp>

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

void PhasedPWM::set_duty_cycle(uint8_t duty_cycle) {
	this->duty_cycle = duty_cycle;
	uint32_t period = peripheral->handle->Instance->ARR;

	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(peripheral->handle) / 100.0 * duty_cycle);

	__HAL_TIM_SET_COMPARE(peripheral->handle, channel, raw_duty + phase*raw_duty / 100.0);

	if (channel % 8) {
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel - 4, raw_duty - phase*raw_duty / 100.0);
	} else {
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel + 4, raw_duty);
	}
}

void PhasedPWM::set_phase(int8_t phase) {
	this->phase = phase;
	set_duty_cycle(duty_cycle);
}
