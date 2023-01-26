/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#include "Encoder/Encoder.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

uint8_t Encoder::id_counter = 0;
map<pair<Pin, Pin>, TimerPeripheral*> Encoder::pin_timer_map;
map<uint8_t, pair<Pin, Pin>> Encoder::registered_encoder;

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
	for (pair<uint8_t, pair<Pin, Pin>> instance : registered_encoder) {
			init(Encoder::pin_timer_map[instance.second]);
	}
}

void Encoder::turn_on(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TimerPeripheral* timer = pin_timer_map[registered_encoder[id]];

	if (HAL_TIM_Encoder_GetState(timer->handle) == HAL_TIM_STATE_RESET) {
		return; //TODO: Exception handle, Error (Encoder not initialized)

	}

	if (HAL_TIM_Encoder_Start(timer->handle, TIM_CHANNEL_ALL) != HAL_OK) {
		return; //TODO: Exception handle, Warning (Error starting encoder)
	}
}

void Encoder::turn_off(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TimerPeripheral* timer = pin_timer_map[registered_encoder[id]];

	if (HAL_TIM_Encoder_Stop(timer->handle, TIM_CHANNEL_ALL) != HAL_OK) {
		//TODO: Exception handle, Warning (Error stopping encoder)
	}
}

void Encoder::reset(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return; //TODO: error handler
	}

	TimerPeripheral* timer =  pin_timer_map[registered_encoder[id]];

	timer->handle->Instance->CNT = UINT16_MAX / 2;
}

optional<uint32_t> Encoder::get_counter(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return nullopt; //TODO: error handler
	}

	TimerPeripheral* timer = pin_timer_map[registered_encoder[id]];

	return timer->handle->Instance->CNT;
}

optional<bool> Encoder::get_direction(uint8_t id){
	if (not Encoder::registered_encoder.contains(id)) {
		return nullopt; //TODO: error handler
	}

	TimerPeripheral* timer =  pin_timer_map[registered_encoder[id]];

	return ((timer->handle->Instance->CR1 & 0b10000) >> 4);
}

void Encoder::init(TimerPeripheral* encoder){
	  TIM_Encoder_InitTypeDef sConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  encoder->handle->Instance = encoder->init_data.timer;
	  encoder->handle->Init.Prescaler = encoder->init_data.prescaler;
	  encoder->handle->Init.CounterMode = TIM_COUNTERMODE_UP;
	  encoder->handle->Init.Period = encoder->init_data.period;
	  encoder->handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  encoder->handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	  sConfig.IC1Filter = 0;
	  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	  sConfig.IC2Filter = 0;
	  if (HAL_TIM_Encoder_Init(encoder->handle, &sConfig) != HAL_OK){
		  //TODO: Error handler
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(encoder->handle, &sMasterConfig) != HAL_OK){
		  //TODO: Error handler
	  }
}

#endif

