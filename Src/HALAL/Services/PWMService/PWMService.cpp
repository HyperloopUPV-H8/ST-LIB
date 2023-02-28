/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: alejandro
 */

#include "PWMService/PWMService.hpp"
#include "ErrorHandler/ErrorHandler.hpp"


uint8_t PWMservice::id_counter = 0;
map<uint8_t, pair<PWMservice::PWM_MODE, PWMvariant>> PWMservice::active_instances = {};
PWMservice::Instance::Instance(TimerPeripheral* peripheral, uint32_t channel, PWMservice::PWM_MODE mode) :
		peripheral(peripheral), channel(channel), mode(mode), duty_cycle(0) {}

optional<uint8_t> PWMservice::inscribe(Pin& pin){
	if (not available_instances.contains(pin)) {
		ErrorHandler("Pin %s is not configured as a PWM in you configuration file", pin.to_string().c_str());
		return nullopt;
	}

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	active_instances[id_counter] = {PWM_MODE::NORMAL, available_instances[pin]};

	TimerPeripheral::InitData& init_data = available_instances[pin].peripheral->init_data;
	init_data.pwm_channels.push_back(available_instances[pin].channel);
	return id_counter++;
}

optional<uint8_t> PWMservice::inscribe_dual(Pin& pin, Pin& pin_negated){
	if (not available_instances_dual.contains({pin, pin_negated})) {
		return nullopt;
		ErrorHandler("Pin %s and Pin %s are not configured as a dual PWM in you configuration file", pin.to_string().c_str(), pin_negated.to_string().c_str());
	} 	
	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);
	Pin::inscribe(pin_negated, TIMER_ALTERNATE_FUNCTION);

	uint8_t real_id = id_counter+32;
	id_counter++;
	active_instances[real_id] = {PWM_MODE::DUAL, available_instances_dual[{pin, pin_negated}]};

	TimerPeripheral::InitData& init_data = available_instances_dual[{pin, pin_negated}].peripheral->init_data;
	init_data.pwm_channels.push_back(available_instances_dual[{pin, pin_negated}].channel);
	return real_id;
}

optional<uint8_t> PWMservice::inscribe_phased(Pin& pin) {
	if (not available_instances_phased.contains(pin)) {
		ErrorHandler("Pin %s is not configured as a phased PWM in you configuration file", pin.to_string().c_str());
		return nullopt;
	}

	Pin::inscribe(pin, TIMER_ALTERNATE_FUNCTION);

	uint8_t real_id = id_counter+64;
	id_counter++;
	active_instances[real_id] = {PWM_MODE::PHASED, available_instances[pin]};

	TimerPeripheral::InitData& init_data = available_instances[pin].peripheral->init_data;
	init_data.pwm_channels.push_back(available_instances[pin].channel);
	return id_counter++;
}

optional<uint8_t> PWMservice::inscribe_dual_phased(Pin& pin) {
	if (not available_instances_phased_dual.contains({pin, pin_negated})) {
		return nullopt;
		ErrorHandler("Pin %s and Pin %s are not configured as a dual phased PWM in you configuration file", pin.to_string().c_str(), pin_negated.to_string().c_str());
	}

	uint8_t real_id = id_counter+96;
	id_counter++;
	active_instances[real_id] = {PWM_MODE::PHASED_DUAL, available_instances[pin]};

	TimerPeripheral::InitData& init_data = available_instances[pin].peripheral->init_data;
	init_data.pwm_channels.push_back(available_instances[pin].channel);
	return id_counter++;
}

void PWMservice::turn_on(constexpr uint8_t id) {
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	constexpr uint8_t pwm_type = id / 32;
	get<pwm_type>(get_instance(id)).turn_on();
}

void PWMservice::turn_off(uint8_t id){
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get<get_type(id)>(get_instance(id)).turn_off();
}

void PWMservice::set_duty_cycle(uint8_t id, float duty_cycle) {

	if (not (duty_cycle >= 0 && duty_cycle <= 100)) {
		ErrorHandler("The duty cycle of value %d must be in the range [0, 100]", duty_cycle);
		return;
	}
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get<get_type(id)>(get_instance(id)).set_duty_cycle();
}

void PWMservice::set_frequency(uint8_t id, uint32_t frequency) {
	if (not instance_exists(id)) {
		ErrorHandler("Instance with id %d does not exist", id);
		return;
	}

	get<get_type(id)>(get_instance(id)).set_frequency();
}

bool PWMservice::instance_exists(uint8_t id) {
	if (PWMservice::active_instances.contains(id)) {
		return true;
	}

	return false;
}

PWMvariant& PWMservice::get_instance(uint8_t id) {
	return PWMservice::active_instances[id].second;
}

constexpr PWMservice::PWM_MODE PWMservice::get_type(uint8_t id) {
	return PWMservice::active_instances[id].first;
}
