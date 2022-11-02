/*
 * InputCapture.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: aleja
 */

#include "InputCapture/InputCapture.hpp"

extern TIM_HandleTypeDef htim2;

forward_list<uint8_t> IC::IDmanager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint32_t, uint32_t> IC::channel_dict = {
		{HAL_TIM_ACTIVE_CHANNEL_1, TIM_CHANNEL_1},
		{HAL_TIM_ACTIVE_CHANNEL_2, TIM_CHANNEL_2},
		{HAL_TIM_ACTIVE_CHANNEL_3, TIM_CHANNEL_3},
		{HAL_TIM_ACTIVE_CHANNEL_4, TIM_CHANNEL_4},
		{HAL_TIM_ACTIVE_CHANNEL_5, TIM_CHANNEL_5},
		{HAL_TIM_ACTIVE_CHANNEL_6, TIM_CHANNEL_6}
};

map<Pin, TimerChannel> IC::pin_timer_map = {
		{PA0, {&htim2, TIM_CHANNEL_1}}
};
map<uint8_t,Pin> IC::service_ids = {};
map<TimerChannel, pair<int, int>> IC::data = {};

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	TimerChannel tim_ch = {htim, IC::channel_dict[htim->Channel]};
	if (IC::data.find(tim_ch) == IC::data.end()) {
		IC::data[tim_ch] = {0, 0} ;
	}
			IC::data[tim_ch].first = IC::data[tim_ch].second;
			IC::data[tim_ch].second = HAL_TIM_ReadCapturedValue(tim_ch.timer, tim_ch.channel);
}

optional<uint8_t> IC::register_ic(Pin& pin){
 	if (IC::pin_timer_map.find(pin) == IC::pin_timer_map.end()) { return {}; }
	Pin::register_pin(pin, ALTERNATIVE);
	uint8_t id = IC::IDmanager.front();
	IC::service_ids[id] = pin;
	IC::IDmanager.pop_front();
	return id;
}

void IC::unregister_ic(uint8_t id){
	Pin::unregister_pin(IC::service_ids[id]);
	IC::service_ids.erase(id);
	IC::IDmanager.push_front(id);
}

void IC::turn_off_ic(uint8_t id){
	Pin pin = IC::service_ids[id];
	TimerChannel tim_ch = IC::pin_timer_map[pin];
	HAL_TIM_IC_Stop_IT(tim_ch.timer, tim_ch.channel);
}

void IC::turn_on_ic(uint8_t id){
	Pin pin = IC::service_ids[id];
	TimerChannel tim_ch = IC::pin_timer_map[pin];
	HAL_TIM_IC_Start_IT(tim_ch.timer, tim_ch.channel);
}

float IC::read_frequency(uint8_t id) {
	Pin pin = IC::service_ids[id];
	TimerChannel tim_ch = IC::pin_timer_map[pin];
	pair <int, int> tim_data = IC::data[tim_ch];

	long int diff;
	if (tim_data.second >= tim_data.first) {
		diff = tim_data.second - tim_data.first;
	}

	else if (tim_data.first > tim_data.second) {
		diff = (0xffffffff - tim_data.first) + tim_data.second;
	}

	float refClock = HAL_RCC_GetPCLK1Freq() / tim_ch.timer->Init.Prescaler;
	return refClock / diff;
}

uint8_t IC::read_duty_cycle(uint8_t id) {
	Pin pin = IC::service_ids[id];
	TimerChannel tim_ch = IC::pin_timer_map[pin];
	pair <int, int> tim_data = IC::data[tim_ch];

	int diff;
	if (tim_data.second >= tim_data.first) {
		diff = tim_data.second - tim_data.first;;
	}

	else if (tim_data.first > tim_data.second) {
		diff = (0xffffffff - tim_data.first) + tim_data.second;
	}

	float refClock = HAL_RCC_GetPCLK1Freq() / tim_ch.timer->Init.Prescaler;
    return (uint8_t)(((uint64_t) diff * 1000000) / refClock);
}



