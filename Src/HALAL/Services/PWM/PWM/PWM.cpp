/*
 * PWM.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include <PWM/PWM/PWM.hpp>
#include "ErrorHandler/ErrorHandler.hpp"
#include "stm32h7xx_hal_def.h"

PWM::PWM(Pin& pin) {
	if (not TimerPeripheral::available_pwm.contains(pin)) {
		ErrorHandler("Pin %s is not registered as an available PWM", pin.to_string());
		return;
	}

	TimerPeripheral& timer = TimerPeripheral::available_pwm.at(pin).first;
	TimerPeripheral::PWMData& pwm_data = TimerPeripheral::available_pwm.at(pin).second;

	if (pwm_data.mode != TimerPeripheral::PWM_MODE::NORMAL) {
		ErrorHandler("Pin %s is not registered as a NORMAL PWM", pin.to_string());
	}

	peripheral = &timer;
	channel = pwm_data.channel;

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	timer.init_data.pwm_channels.push_back(pwm_data);

	duty_cycle = 0;
}

void PWM::turn_on() {
  if(is_on) return;
  if(HAL_TIM_PWM_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("PWM did not turn on correctly", 0);
  }
  is_on = true;
}

void PWM::turn_off() {
  if(not is_on) return;
  if(HAL_TIM_PWM_Stop(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("PWM did not stop correctly", 0);
  }
  is_on = false;
}

void PWM::set_duty_cycle(float duty_cycle) {
	uint16_t raw_duty = __HAL_TIM_GET_AUTORELOAD(peripheral->handle) / 100.0 * duty_cycle;
	__HAL_TIM_SET_COMPARE(peripheral->handle, channel, raw_duty);
	this->duty_cycle = duty_cycle;
}

void PWM::set_frequency(uint32_t frequency) {
	this->frequency = frequency;
	TIM_TypeDef& timer = *peripheral->handle->Instance;
	timer.ARR = (HAL_RCC_GetPCLK1Freq()*2 / (timer.PSC+1)) / frequency;
	set_duty_cycle(duty_cycle);
}

uint32_t PWM::get_frequency() {
	return frequency;
}

float PWM::get_duty_cycle(){
	return duty_cycle;
}
