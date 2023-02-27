/*
 * DualPWMInstance.cpp
 *
 *  Created on: Feb 27, 2023
 *      Author: aleja
 */

#include "PWMService/DualPWMInstance/DualPWMInstance.hpp"

void DualPWMInstance::turn_on() {
	HAL_TIM_PWM_Start(peripheral->handle, channel);
	HAL_TIMEx_PWMN_Start(peripheral->handle, channel);
}

void DualPWMInstance::turn_off() {
	HAL_TIM_PWM_Stop(peripheral->handle, channel);
	HAL_TIMEx_PWMN_Stop(peripheral->handle, channel);
}
