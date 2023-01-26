/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro
 */

#include "PWMService/PWMService.hpp"

uint8_t PWMservice::id_counter = 0;
map<uint8_t, PWMservice::Instance> PWMservice::active_instances = {};
PWMservice::Instance::Instance(TimerPeripheral* peripheral, uint32_t channel, PWMservice::Mode mode) :
		peripheral(peripheral), channel(channel), mode(mode) {}

optional<uint8_t> PWMservice::inscribe(Pin& pin){
	if (not available_instances.contains(pin)) {
		ErrorHandler("Pin %s is not configured as a PWM in you configuration file", pin.to_string());
		return nullopt;
	}

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	active_instances[id_counter] = available_instances[pin];

	TimerPeripheral::InitData& init_data = active_instances[id_counter].peripheral->init_data;
	init_data.pwm_channels.push_back(active_instances[id_counter].channel);
	return id_counter++;
}

optional<uint8_t> PWMservice::inscribe_negated(Pin& pin) {
	if (not available_instances_negated.contains(pin)) {
		return nullopt;
		ErrorHandler("Pin %s is not configured as a PWM in you configuration file", pin.to_string());
	} 	
	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	active_instances[id_counter] = available_instances_negated[pin];

	TimerPeripheral::InitData& init_data = active_instances[id_counter].peripheral->init_data;
	init_data.pwm_channels.push_back(active_instances[id_counter].channel);
 	return id_counter++;
}

optional<uint8_t> PWMservice::inscribe_dual(Pin& pin, Pin& pin_negated){
	if (not available_instances_dual.contains({pin, pin_negated})) {
		return nullopt;
		ErrorHandler("Pin %s and Pin %s are not configured as a dual PWM in you configuration file", pin.to_string(), pin_negated.to_string());
	} 	
	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	Pin::inscribe(pin_negated, TIMER_ALTERNATE_FUNCTION);
	active_instances[id_counter] = available_instances_dual[{pin, pin_negated}];

	TimerPeripheral::InitData& init_data = active_instances[id_counter].peripheral->init_data;
	init_data.pwm_channels.push_back(active_instances[id_counter].channel);
	return id_counter++;
}

void PWMservice::turn_on(uint8_t id) {
	if (not instance_exists(id)) {
		Error_Handler("Instance with id %d does not exist", id);
		return;
	}

	Instance& instance = get_instance(id);

	if (instance.mode == NORMAL) {
		HAL_TIM_PWM_Start(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == NEGATED) {
		HAL_TIMEx_PWMN_Start(instance.peripheral->handle, instance.channel);
	}

	else if(instance.mode == DUAL) {
		HAL_TIM_PWM_Start(instance.peripheral->handle, instance.channel);
		HAL_TIMEx_PWMN_Start(instance.peripheral->handle, instance.channel);
	}
}

void PWMservice::turn_off(uint8_t id){
	if (not instance_exists(id)) {
		Error_Handler("Instance with id %d does not exist", id);
		return;
	}

	Instance& instance = get_instance(id);
	if (instance.mode == NORMAL) {
		HAL_TIM_PWM_Stop(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == NEGATED) {
		HAL_TIMEx_PWMN_Stop(instance.peripheral->handle, instance.channel);
	}

	else if (instance.mode == DUAL) {
		HAL_TIM_PWM_Stop(instance.peripheral->handle, instance.channel);
		HAL_TIMEx_PWMN_Stop(instance.peripheral->handle, instance.channel);
	}

	else {
		Error_Handler("This should never be activated");
	}
}

void PWMservice::set_duty_cycle(uint8_t id, uint8_t duty_cycle) {
	if (not (duty_cycle >= 0 && duty_cycle <= 100)) {
		Error_Handler("The duty cycle of value %d must be in the range [0, 100]", duty_cycle);
		return;
	}
	if (not instance_exists(id)) {
		Error_Handler("Instance with id %d does not exist", id);
		return;
	}

	Instance& instance = get_instance(id);
	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(instance.peripheral->handle) / 100.0 * duty_cycle);
	__HAL_TIM_SET_COMPARE(instance.peripheral->handle, instance.channel, raw_duty);
}

bool PWMservice::instance_exists(uint8_t id) {
	if (PWMservice::active_instances.contains(id)) {
		return true;
	}

	return false;
}

PWMservice::Instance& PWMservice::get_instance(uint8_t id) {
	return PWMservice::active_instances[id];
}


