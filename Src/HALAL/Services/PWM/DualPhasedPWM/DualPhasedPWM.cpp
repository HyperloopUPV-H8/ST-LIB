/*
 * DualPhasedPWM.cpp
 *
 *  Created on: 9 mar. 2023
 *      Author: aleja
 */

#include "PWM/DualPhasedPWM/DualPhasedPWM.hpp"

DualPhasedPWM::DualPhasedPWM(Pin& pin, Pin& pin_negated) {
	if (not TimerPeripheral::available_dual_pwms.contains({pin, pin_negated})) {
		ErrorHandler("Pins %s and %s are not registered as an available Dual PHASED PWM", pin.to_string(), pin_negated.to_string());
	}

	TimerPeripheral& timer = TimerPeripheral::available_dual_pwms.at({pin, pin_negated}).first;
	TimerPeripheral::PWMData& pwm_data = TimerPeripheral::available_dual_pwms.at({pin, pin_negated}).second;

	peripheral = &timer;
	channel = pwm_data.channel;

	if (pwm_data.mode != TimerPeripheral::PWM_MODE::PHASED) {
		ErrorHandler("Pins %s and %s are not registered as a DUAL PHASED PWM", pin.to_string(), pin_negated.to_string());
	}

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	Pin::inscribe(pin_negated, TIMER_ALTERNATE_FUNCTION);
	timer.init_data.pwm_channels.push_back(pwm_data);

	duty_cycle = 0;
}
void DualPhasedPWM::set_duty_cycle(float duty_cycle){
	if (channel % 8 == 1) {
		//odd register controls count up
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel-4, 0);
		//even register controls count down
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel, __HAL_TIM_GET_AUTORELOAD(peripheral->handle));

	}else{
		//odd register controls count up
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel, 0);
		//even register controls count down
		__HAL_TIM_SET_COMPARE(peripheral->handle, channel + 4, __HAL_TIM_GET_AUTORELOAD(peripheral->handle));
	}
	this->duty_cycle = duty_cycle;
}
void DualPhasedPWM::turn_on() {
  if (HAL_TIM_PWM_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM positive channel did not start correctly", 0);
  }

  if (HAL_TIMEx_PWMN_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM negative channel did not start correctly", 0);
  }
}