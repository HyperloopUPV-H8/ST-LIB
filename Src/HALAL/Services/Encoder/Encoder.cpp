/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#include "Runes/Runes.hpp"

extern TIM_HandleTypeDef htim8;

optional<uint8_t> Encoder::inscribe(Pin& pin1, Pin& pin2){
	pair<Pin, Pin> doublepin = {pin1, pin2};
	if (not pin_timer_map.contains(doublepin)) {
		return nullopt;
	}

	Pin::inscribe(pin1, ALTERNATIVE);
	Pin::inscribe(pin2, ALTERNATIVE);

	uint8_t id = Encoder::id_manager.front();
	Encoder::service_ids[id] = doublepin;

	Encoder::id_manager.pop_front();
	return id;
}


void Encoder::turn_on(uint8_t id){
	if (not Encoder::service_ids.contains(id)) {
		return; // TODO error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[service_ids[id]];

	if (HAL_TIM_Encoder_GetState(timer) == HAL_TIM_STATE_RESET) {
		//TODO: Exception handle, Error (Encoder not initialized)
		return;
	}

	if (HAL_TIM_Encoder_Start(timer, TIM_CHANNEL_ALL) != HAL_OK) {
		//TODO: Exception handle, Warning (Error starting encoder)
	}
}

void Encoder::turn_off(uint8_t id){
	if (not Encoder::service_ids.contains(id)) {
		return; // TODO error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[service_ids[id]];

	if (HAL_TIM_Encoder_Stop(timer, TIM_CHANNEL_ALL) != HAL_OK) {
		// TODO: Exception handle, Warning (Error stopping encoder)
	}
}

void Encoder::reset(uint8_t id){
	if (not Encoder::service_ids.contains(id)) {
		return; // TODO error handler
	}

	TIM_HandleTypeDef* timer =  pin_timer_map[service_ids[id]];

	timer->Instance->CNT = 0;
}

optional<uint32_t> Encoder::get_counter(uint8_t id){
	if (not Encoder::service_ids.contains(id)) {
		return nullopt; // TODO error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[service_ids[id]];

	return timer->Instance->CNT;
}

optional<bool> Encoder::get_direction(uint8_t id){
	if (not Encoder::service_ids.contains(id)) {
		return nullopt; // TODO error handler
	}

	TIM_HandleTypeDef* timer =  pin_timer_map[service_ids[id]];

	return ((timer->Instance->CR1 & 0b10000) >> 4);
}
