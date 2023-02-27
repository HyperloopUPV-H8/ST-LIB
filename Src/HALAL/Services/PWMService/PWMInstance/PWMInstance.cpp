/*
 * PWMInstance.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include "PWMService/PWMInstance/PWMInstance.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

PWMInstance::PWMInstance(TimerPeripheral* peripheral, uint32_t channel) :
		peripheral(peripheral), channel(channel), duty_cycle(0) {}

void PWMInstance::turn_on() {
	HAL_TIM_PWM_Start(peripheral->handle, channel);
}

void PWMInstance::turn_off() {
	HAL_TIM_PWM_Stop(peripheral->handle, channel);
}

void PWMInstance::set_duty_cycle(uint8_t duty_cycle) {
	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(peripheral->handle) / 100.0 * duty_cycle);
	__HAL_TIM_SET_COMPARE(peripheral->handle, channel, raw_duty);
}

void PWMInstance::set_frequency(uint32_t frequency) {
	TIM_TypeDef& timer = *peripheral->handle->Instance;
	timer.ARR = (HAL_RCC_GetPCLK1Freq()*2 / (timer.PSC+1)) / frequency;
	set_duty_cycle(duty_cycle);
}

void PWMInstance::set_phase(uint8_t phase) {
	ErrorHandler("This PWM instance does not support phasing");
}
