/*
 * Encoder.cpp
 *
 *  Created on: 27 oct. 2022
 *      Author: Pablo
 */

//TODO: arreglar include antes del PR
#include "../Inc/HALAL/Services/Encoder/Encoder.hpp"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim23;
extern TIM_HandleTypeDef htim24;

forward_list<uint8_t> Encoder::ID_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
//TODO: Change this map when the runes are finished (this map is only for test purpose)
map<pair<Pin, Pin>, DoubleTimerChannel> Encoder::pin_timer_map = {
		{pair<Pin, Pin>(PE9, PE11), {&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2}},
		{pair<Pin, Pin>(PA6, PB5), {&htim3, TIM_CHANNEL_1, TIM_CHANNEL_2}},
		{pair<Pin, Pin>(PB5, PE11), {&htim4, TIM_CHANNEL_1, TIM_CHANNEL_2}},
		{pair<Pin, Pin>(PC6, PC7), {&htim8, TIM_CHANNEL_1, TIM_CHANNEL_2}},
		{pair<Pin, Pin>(PF0, PF1), {&htim23, TIM_CHANNEL_1, TIM_CHANNEL_2}},
		{pair<Pin, Pin>(PF11, PF12), {&htim24, TIM_CHANNEL_1, TIM_CHANNEL_2}},
};

map<uint8_t, pair<Pin, Pin>> Encoder::service_IDs = {};

optional<uint8_t> register_encoder(Pin& pin1, Pin& pin2){
	if (Encoder::pin_timer_map.find(pair<Pin, Pin>(pin1, pin2)) == Encoder::pin_timer_map.end())
		return {};

	Pin::register_pin(pin1, ALTERNATIVE);
	Pin::register_pin(pin2, ALTERNATIVE);

	uint8_t id = Encoder::ID_manager.front();
	Encoder::service_IDs[id] = pair(pin1, pin2);

	Encoder::ID_manager.pop_front();
	return id;
}

void turn_on_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	Pin pin1 = Encoder::service_IDs[id].first;
	Pin pin2 = Encoder::service_IDs[id].second;

	DoubleTimerChannel timer = Encoder::pin_timer_map[pair<Pin, Pin>(pin1, pin2)];

	if (HAL_TIM_Encoder_GetState(timer.timer) == HAL_TIM_STATE_RESET) {
		//TODO: Exception handle, Error (Encoder not initialized)
		return;
	}

	if (HAL_TIM_Encoder_Start(timer.timer, timer.channel1) != HAL_OK ||
		HAL_TIM_Encoder_Start(timer.timer, timer.channel2) != HAL_OK	) {
		//TODO: Exception handle, Warning (Error starting encoder)
	}
}

void turn_off_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	Pin pin1 = Encoder::service_IDs[id].first;
	Pin pin2 = Encoder::service_IDs[id].second;

	DoubleTimerChannel timer = Encoder::pin_timer_map[pair<Pin, Pin>(pin1, pin2)];

	if (HAL_TIM_Encoder_Stop(timer.timer, timer.channel1) != HAL_OK ||
		HAL_TIM_Encoder_Stop(timer.timer, timer.channel2) != HAL_OK	) {
		//TODO: Exception handle, Warning (Error stoping encoder)
	}
}

void reset_encoder(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return;

	Pin pin1 = Encoder::service_IDs[id].first;
	Pin pin2 = Encoder::service_IDs[id].second;
	DoubleTimerChannel timer = Encoder::pin_timer_map[pair<Pin, Pin>(pin1, pin2)];

	timer.timer->Instance->CNT = 0;
}

optional<uint32_t> get_encoder_counter(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return{};

	Pin pin1 = Encoder::service_IDs[id].first;
	Pin pin2 = Encoder::service_IDs[id].second;
	DoubleTimerChannel timer = Encoder::pin_timer_map[pair<Pin, Pin>(pin1, pin2)];

	return timer.timer->Instance->CNT;
}

optional<bool> get_encoder_direction(uint8_t id){
	if (!Encoder::service_IDs.contains(id))
		return{};

	Pin pin1 = Encoder::service_IDs[id].first;
	Pin pin2 = Encoder::service_IDs[id].second;
	DoubleTimerChannel timer = Encoder::pin_timer_map[pair<Pin, Pin>(pin1, pin2)];

	return (timer.timer->Instance->CR1 & 0b10000) >> 4;
}
