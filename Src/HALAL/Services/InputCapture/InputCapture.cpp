/*
 * InputCapture.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: aleja
 */

#include "InputCapture/InputCapture.hpp"

extern TIM_HandleTypeDef htim2;

forward_list<uint8_t> IC::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint32_t, uint32_t> IC::channel_dict = {
		{HAL_TIM_ACTIVE_CHANNEL_1, TIM_CHANNEL_1},
		{HAL_TIM_ACTIVE_CHANNEL_2, TIM_CHANNEL_2},
		{HAL_TIM_ACTIVE_CHANNEL_3, TIM_CHANNEL_3},
		{HAL_TIM_ACTIVE_CHANNEL_4, TIM_CHANNEL_4},
		{HAL_TIM_ACTIVE_CHANNEL_5, TIM_CHANNEL_5},
		{HAL_TIM_ACTIVE_CHANNEL_6, TIM_CHANNEL_6}
};

map<Pin, TimerChannelRisingFalling> IC::pin_timer_map = {
		{PA0, {&htim2, TIM_CHANNEL_1, TIM_CHANNEL_2}}
};
map<uint8_t, Pin> IC::service_ids = {};
map<TimerChannelRisingFalling, IC::data> IC::data_map = {};

optional<uint8_t> IC::register_ic(Pin& pin){
 	if (!IC::pin_timer_map.contains(pin)) { return {}; }

 	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
 	IC::data ic_data = {{ 0 }, 0, 0};
 	IC::data_map[tim_ch] = ic_data;

	Pin::register_pin(pin, ALTERNATIVE);
	uint8_t id = IC::id_manager.front();
	IC::service_ids[id] = pin;
	IC::id_manager.pop_front();
	return id;
}

void IC::unregister_ic(uint8_t id){
	Pin pin = IC::service_ids[id];
	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
 	IC::data_map.erase(tim_ch);

	Pin::unregister_pin(IC::service_ids[id]);
	IC::service_ids.erase(id);
	IC::id_manager.push_front(id);
}

void IC::turn_off_ic(uint8_t id){
	Pin pin = IC::service_ids[id];
	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
	HAL_TIM_IC_Stop_IT(tim_ch.timer, tim_ch.channel_rising);
	HAL_TIM_IC_Stop_IT(tim_ch.timer, tim_ch.channel_falling);
}

void IC::turn_on_ic(uint8_t id){
	Pin pin = IC::service_ids[id];
	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
	HAL_TIM_IC_Start_IT(tim_ch.timer, tim_ch.channel_rising);
	HAL_TIM_IC_Start_IT(tim_ch.timer, tim_ch.channel_falling);
}

float IC::read_frequency(uint8_t id) {
	Pin pin = IC::service_ids[id];
	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
	IC::data ic_data = data_map[tim_ch];

	return ic_data.frequency;
}

uint8_t IC::read_duty_cycle(uint8_t id) {
	Pin pin = IC::service_ids[id];
	TimerChannelRisingFalling tim_ch = IC::pin_timer_map[pin];
	IC::data ic_data = IC::data_map[tim_ch];
	return ic_data.duty_cycle;
}

ChannelsRisingFalling IC::find_other_channel(uint32_t channel) {
	for (auto pin_timer : IC::pin_timer_map) {
		uint32_t& ch_rising = pin_timer.second.channel_rising;
		uint32_t& ch_falling = pin_timer.second.channel_falling;

		if(ch_rising == channel || ch_falling == channel) {
			return {ch_rising, ch_falling};
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	ChannelsRisingFalling channels = IC::find_other_channel(IC::channel_dict[htim->Channel]);
	TimerChannelRisingFalling tim_ch = {htim, channels.rising, channels.falling};
	IC::data& ic_data = IC::data_map[tim_ch];

	// Rising edge skip error handle
	if (IC::channel_dict[htim->Channel] == channels.rising && ic_data.count % 2 == 1) {
		ic_data.count = 0;
		return;
	}

	// Falling edge skip error handle
	if (IC::channel_dict[htim->Channel] == channels.falling && ic_data.count % 2 == 0) {
		ic_data.count = 0;
		return;
	}

	switch (ic_data.count) {
		case 0: // First Rising edge
			ic_data.counter_values[0] = HAL_TIM_ReadCapturedValue(tim_ch.timer, tim_ch.channel_rising);
			ic_data.count++;
			break;

		case 1: // First Falling edge
			ic_data.counter_values[1] = HAL_TIM_ReadCapturedValue(tim_ch.timer, tim_ch.channel_falling);
			ic_data.count++;
			break;

		case 2: // Second rising edge
			ic_data.counter_values[2] = HAL_TIM_ReadCapturedValue(tim_ch.timer, tim_ch.channel_rising);
			ic_data.count++;
			break;

		case 3: // Second falling edge (Redundant)
			ic_data.counter_values[3] = HAL_TIM_ReadCapturedValue(tim_ch.timer, tim_ch.channel_falling);

			//Reference clock of the IC timer calculation by dividing the clk frequency by the timer prescaler.
			uint32_t refClock = HAL_RCC_GetPCLK1Freq()*2 / (tim_ch.timer->Init.Prescaler+1);

			uint32_t diff;

			if (ic_data.counter_values[2] > ic_data.counter_values[0]) {
				diff = ic_data.counter_values[2] - ic_data.counter_values[0];
			}

			// Overflow handling
			else if (ic_data.counter_values[0] > ic_data.counter_values[2]) {
				diff = (IC_OVERFLOW - ic_data.counter_values[0]) + ic_data.counter_values[2];
			}

			// diff/refClock gives the real time between the two edges, so refClock/diff gives freq.
			ic_data.frequency = refClock/diff;

			float duty_diff;
			if (ic_data.counter_values[1] > ic_data.counter_values[0]) {
				duty_diff = ic_data.counter_values[1] - ic_data.counter_values[0];
			}

			// Overflow handling
			else if (ic_data.counter_values[1] > ic_data.counter_values[2]) {
				duty_diff = (IC_OVERFLOW - ic_data.counter_values[0]) + ic_data.counter_values[1];
			}

			// duty_diff represents the amount of time the pin was high, dividing it between the whole period gives the percentage.
			ic_data.duty_cycle = duty_diff / diff * 100;
			ic_data.count = 0;
			break;
	}
}


