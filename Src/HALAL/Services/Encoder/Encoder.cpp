/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

#include "Encoder/Encoder.hpp"

extern TIM_HandleTypeDef htim8;
forward_list<uint8_t> Encoder::ID_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

map<pair<Pin, Pin>, TIM_HandleTypeDef*> Encoder::pin_timer_map = {
		{{PC6, PC7}, {&htim8}},
};

map<uint8_t, pair<Pin, Pin>> Encoder::service_IDs = {};


optional<uint8_t> Encoder::register_encoder(Pin& pin1, Pin& pin2){
	pair<Pin, Pin> doublepin = {pin1, pin2};
	if (!Encoder::pin_timer_map.contains(doublepin))
		return nullopt;

	Pin::register_pin(pin1, ALTERNATIVE);
	Pin::register_pin(pin2, ALTERNATIVE);

	uint8_t id = Encoder::ID_manager.front();
	Encoder::service_IDs[id] = doublepin;

	Encoder::ID_manager.pop_front();
	return id;
}


void Encoder::turn_on_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	TIM_HandleTypeDef* timer = Encoder::pin_timer_map[service_IDs[id]];

	if (HAL_TIM_Encoder_GetState(timer) == HAL_TIM_STATE_RESET) {
		//TODO: Exception handle, Error (Encoder not initialized)
		return;
	}

	if (HAL_TIM_Encoder_Start(timer, TIM_CHANNEL_ALL) != HAL_OK) {
		//TODO: Exception handle, Warning (Error starting encoder)
	}
}

void Encoder::turn_off_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	TIM_HandleTypeDef* timer =  Encoder::pin_timer_map[service_IDs[id]];

	if (HAL_TIM_Encoder_Stop(timer, TIM_CHANNEL_ALL) != HAL_OK	) {
		//TODO: Exception handle, Warning (Error stopping encoder)
	}
}

void Encoder::reset_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	TIM_HandleTypeDef* timer =  Encoder::pin_timer_map[service_IDs[id]];

	timer->Instance->CNT = 32768;
}

optional<uint32_t> Encoder::get_encoder_counter(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return nullopt;

	TIM_HandleTypeDef* timer = Encoder::pin_timer_map[service_IDs[id]];

	return timer->Instance->CNT;
}

optional<bool> Encoder::get_encoder_direction(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return nullopt;

	TIM_HandleTypeDef* timer =  Encoder::pin_timer_map[service_IDs[id]];

	return ((timer->Instance->CR1 & 0b10000) >> 4);
}
