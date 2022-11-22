/*
 * PWM_Module.cpp
 *
 *  Created on: 19 oct. 2022
 *      Author: aleja
 */

#include <Runes/Runes.hpp>

optional<uint8_t> PWM::inscribe(Pin& pin){
	if (!PWM::pin_timer_map.contains(pin)) {
		return nullopt;
	} 	// TODO error handling
	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = PWM::id_manager.front();
	PWM::service_ids[id] = pin;
	PWM::id_manager.pop_front();
	return id;
}

optional<uint8_t> PWM::inscribe_negated(Pin& pin) {
	if (not pin_timer_map_negated.contains(pin)) {
		return nullopt;
	} 	// TODO error handling
	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = PWM::id_manager.front();
	PWM::service_ids_negated[id] = pin;
	PWM::id_manager.pop_front();
 	return id;
}

optional<uint8_t> PWM::inscribe_dual(Pin& pin, Pin& pin_negated){
	if (not PWM::pin_timer_map_dual.contains({pin, pin_negated})) {
		return nullopt;
	} 	// TODO error handling
	Pin::inscribe(pin, ALTERNATIVE);
	Pin::inscribe(pin_negated, ALTERNATIVE);
	uint8_t id = PWM::id_manager.front();
	PWM::service_ids_dual[id] = {pin, pin_negated};
	PWM::id_manager.pop_front();
	return id;
}

void PWM::turn_on(uint8_t id){
	if (PWM::service_ids.contains(id)) {
		Pin pin = PWM::service_ids[id];
		TimerChannel tim_ch = PWM::pin_timer_map[pin];
		HAL_TIM_PWM_Start(tim_ch.timer, tim_ch.channel);
	}

	else if(PWM::service_ids_negated.contains(id)) {
		Pin pin = PWM::service_ids_negated[id];
		TimerChannel tim_ch = PWM::pin_timer_map_negated[pin];
		HAL_TIMEx_PWMN_Start(tim_ch.timer, tim_ch.channel);
	}

	else if(PWM::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWM::service_ids_dual[id];
		TimerChannel tim_ch = PWM::pin_timer_map_dual[pin_pair];
		HAL_TIM_PWM_Start(tim_ch.timer, tim_ch.channel);
		HAL_TIMEx_PWMN_Start(tim_ch.timer, tim_ch.channel);
	}

	else {
		// TODO error handling
	}
}

void PWM::turn_off(uint8_t id){
	if (PWM::service_ids.contains(id)) {
		Pin pin = PWM::service_ids[id];
		TimerChannel tim_ch = PWM::pin_timer_map[pin];
		HAL_TIM_PWM_Stop(tim_ch.timer, tim_ch.channel);
	}

	else if (PWM::service_ids_negated.contains(id)) {
		Pin pin = PWM::service_ids_negated[id];
		TimerChannel tim_ch = PWM::pin_timer_map_negated[pin];
		HAL_TIMEx_PWMN_Stop(tim_ch.timer, tim_ch.channel);
	}

	else if (PWM::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWM::service_ids_dual[id];
		TimerChannel tim_ch = PWM::pin_timer_map_dual[pin_pair];
		HAL_TIM_PWM_Stop(tim_ch.timer, tim_ch.channel);
		HAL_TIMEx_PWMN_Stop(tim_ch.timer, tim_ch.channel);
	}

	else {
		// TODO Error handling
	}
}

void PWM::set_duty(uint8_t id, uint8_t duty_cycle) {
	if (!(duty_cycle >= 0 && duty_cycle <= 100)) {
		// TODO error handling
		return;
	}

	TimerChannel tim_ch;
	if (PWM::service_ids.contains(id)) {
		Pin pin = PWM::service_ids[id];
		tim_ch = PWM::pin_timer_map[pin];
	}

	else if (PWM::service_ids_negated.contains(id)) {
		Pin pin = PWM::service_ids_negated[id];
		tim_ch = PWM::pin_timer_map_negated[pin];
	}

	else if (PWM::service_ids_dual.contains(id)) {
		pair<Pin, Pin> pin_pair = PWM::service_ids_dual[id];
		tim_ch = PWM::pin_timer_map_dual[pin_pair];
	}
	else {
		// TODO error handling
		return;
	}
	uint16_t raw_duty = round(__HAL_TIM_GET_AUTORELOAD(tim_ch.timer) / 100.0 * duty_cycle);
	__HAL_TIM_SET_COMPARE(tim_ch.timer, tim_ch.channel, raw_duty);
}





