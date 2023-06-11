/*
 * DualPWM.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include "PWM/DualPWM/DualPWM.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "stm32h7xx_hal_def.h"

DualPWM::DualPWM(Pin& pin, Pin& pin_negated) {
	if (not TimerPeripheral::available_dual_pwms.contains({pin, pin_negated})) {
		ErrorHandler("Pins %s and %s are not registered as an available Dual PWM", pin.to_string(), pin_negated.to_string());
	}

	TimerPeripheral& timer = TimerPeripheral::available_dual_pwms.at({pin, pin_negated}).first;
	TimerPeripheral::PWMData& pwm_data = TimerPeripheral::available_dual_pwms.at({pin, pin_negated}).second;

	peripheral = &timer;
	channel = pwm_data.channel;

	if (pwm_data.mode != TimerPeripheral::PWM_MODE::NORMAL) {
		ErrorHandler("Pins %s and %s are not registered as a DUAL PWM", pin.to_string(), pin_negated.to_string());
	}

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	Pin::inscribe(pin_negated, TIMER_ALTERNATE_FUNCTION);
	timer.init_data.pwm_channels.push_back(pwm_data);

	duty_cycle = 0;
}

void DualPWM::turn_on() {
  if (HAL_TIM_PWM_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM positive channel did not start correctly", 0);
  }

  if (HAL_TIMEx_PWMN_Start(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM negative channel did not start correctly", 0);
  }
}

void DualPWM::turn_off() {

  if (HAL_TIM_PWM_Stop(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM positive channel did not stop correctly", 0);
  }

  if (HAL_TIMEx_PWMN_Stop(peripheral->handle, channel) != HAL_OK) {
    ErrorHandler("Dual PWM negative channel did not stop correctly", 0);
  }
}

void DualPWM::turn_on_positive() {
  if (HAL_TIM_PWM_Start(peripheral->handle, channel) != HAL_OK) {
	ErrorHandler("Dual PWM positive channel did not start correctly", 0);
  }
}

void DualPWM::turn_on_negated() {
  if (HAL_TIMEx_PWMN_Start(peripheral->handle, channel) != HAL_OK) {
	ErrorHandler("Dual PWM negative channel did not start correctly", 0);
  }
}

void DualPWM::turn_off_positive() {
  if (HAL_TIM_PWM_Stop(peripheral->handle, channel) != HAL_OK) {
	ErrorHandler("Dual PWM positive channel did not stop correctly", 0);
  }
}

void DualPWM::turn_off_negated() {
  if (HAL_TIMEx_PWMN_Stop(peripheral->handle, channel) != HAL_OK) {
	ErrorHandler("Dual PWM negative channel did not stop correctly", 0);
  }
}


