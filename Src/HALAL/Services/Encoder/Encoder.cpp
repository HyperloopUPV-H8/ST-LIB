/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#include "Encoder/Encoder.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

map<uint8_t, pair<Pin, Pin>> Encoder::registered_encoder = {};

uint8_t Encoder::id_counter = 0;

optional<uint8_t> Encoder::inscribe(Pin& pin1, Pin& pin2){
	pair<Pin, Pin> doublepin = {pin1, pin2};
	if (not Encoder::pin_timer_map.contains(doublepin)) {
		return nullopt;
	}

	Pin::inscribe(pin1, ALTERNATIVE);
	Pin::inscribe(pin2, ALTERNATIVE);

	uint8_t id = Encoder::id_counter++;
	Encoder::registered_encoder[id] = doublepin;

	return id;
}

void Encoder::start(){
	for_each(Encoder::registered_encoder.begin(), Encoder::registered_encoder.end(),
			[](pair<uint8_t, pair<Pin, Pin>> iter) {
				Encoder::init(Encoder::pin_timer_map[iter.second]);
			}
	);
}

void Encoder::turn_on(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[registered_encoder[id]].first;

	if (HAL_TIM_Encoder_GetState(timer) == HAL_TIM_STATE_RESET) {
		return; //TODO: Exception handle, Error (Encoder not initialized)

	}

	if (HAL_TIM_Encoder_Start(timer, TIM_CHANNEL_ALL) != HAL_OK) {
		return; //TODO: Exception handle, Warning (Error starting encoder)
	}
}

void Encoder::turn_off(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[registered_encoder[id]].first;

	if (HAL_TIM_Encoder_Stop(timer, TIM_CHANNEL_ALL) != HAL_OK) {
		//TODO: Exception handle, Warning (Error stopping encoder)
	}
}

void Encoder::reset(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TIM_HandleTypeDef* timer =  pin_timer_map[registered_encoder[id]].first;

	timer->Instance->CNT = UINT16_MAX / 2;
}

optional<uint32_t> Encoder::get_counter(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return nullopt; //TODO: error handler
	}

	TIM_HandleTypeDef* timer = pin_timer_map[registered_encoder[id]].first;

	return timer->Instance->CNT;
}

optional<bool> Encoder::get_direction(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return nullopt; //TODO: error handler
	}

	TIM_HandleTypeDef* timer =  pin_timer_map[registered_encoder[id]].first;

	return ((timer->Instance->CR1 & 0b10000) >> 4);
}

void Encoder::init(pair<TIM_HandleTypeDef*, TIM_TypeDef*> encoder){
	  TIM_Encoder_InitTypeDef sConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  encoder.first->Instance = encoder.second;
	  encoder.first->Init.Prescaler = 0;
	  encoder.first->Init.CounterMode = TIM_COUNTERMODE_UP;
	  encoder.first->Init.Period = 65535;
	  encoder.first->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  encoder.first->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	  sConfig.IC1Filter = 0;
	  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	  sConfig.IC2Filter = 0;
	  if (HAL_TIM_Encoder_Init(encoder.first, &sConfig) != HAL_OK){
		  //TODO: Error handler
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(encoder.first, &sMasterConfig) != HAL_OK){
		  //TODO: Error handler
	  }
}

#endif

