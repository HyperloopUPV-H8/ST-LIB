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
	this->duty_cycle = duty_cycle;
		float raw_phase = phase;
		if(raw_phase > 100.0){
			duty_cycle = 100.0 - duty_cycle;
			raw_phase = raw_phase - 100.0;
			__STLIB_TIM_SET_MODE(peripheral->handle, channel, STLIB_TIMER_CCMR_PWM_MODE_1)
		}else{
			__STLIB_TIM_SET_MODE(peripheral->handle, channel, STLIB_TIMER_CCMR_PWM_MODE_2)
		}
		uint32_t arr = peripheral->handle->Instance->ARR;
		float start_high = arr*(50.0 - duty_cycle)/50.0;
		float end_high = arr*(100.0 - duty_cycle)/50.0 + 1;
		if(start_high < 0){start_high = 0;}
		if(end_high > arr){end_high = arr;}
		float max_range = duty_cycle > 50.0 ? 100 - duty_cycle : duty_cycle;
		start_high = start_high + arr * max_range * raw_phase / 5000.0;
		end_high = end_high - arr * max_range * raw_phase / 5000.0;


		__HAL_TIM_SET_COMPARE(peripheral->handle, channel, start_high);

		if (channel % 8 == 0) {
			__HAL_TIM_SET_COMPARE(peripheral->handle, channel + 4, end_high);
		} else {
			__HAL_TIM_SET_COMPARE(peripheral->handle, channel - 4, end_high);
		}
}


void DualPhasedPWM::set_frequency(uint32_t freq_in_hz){
  	this->frequency = freq_in_hz;
	TIM_TypeDef& timer = *peripheral->handle->Instance;
	timer.ARR = (HAL_RCC_GetPCLK1Freq()*2 / (timer.PSC+1)) / 2  / frequency;
	set_duty_cycle(duty_cycle);
}

void DualPhasedPWM::set_phase(float phase) {
	this->phase = phase;
	set_duty_cycle(duty_cycle);
}

float DualPhasedPWM::get_phase()const{
	return phase;
}
