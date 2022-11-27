/*
 * InputCapture.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: alejandro
 */
#include "InputCapture/InputCapture.hpp"

forward_list<uint8_t> InputCapture::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint8_t, InputCapture::Instance> InputCapture::instances = {};
static map<uint32_t, uint32_t> channel_dict = {
	{HAL_TIM_ACTIVE_CHANNEL_1, TIM_CHANNEL_1},
	{HAL_TIM_ACTIVE_CHANNEL_2, TIM_CHANNEL_2},
	{HAL_TIM_ACTIVE_CHANNEL_3, TIM_CHANNEL_3},
	{HAL_TIM_ACTIVE_CHANNEL_4, TIM_CHANNEL_4},
	{HAL_TIM_ACTIVE_CHANNEL_5, TIM_CHANNEL_5},
	{HAL_TIM_ACTIVE_CHANNEL_6, TIM_CHANNEL_6}
};

InputCapture::Instance::Instance(Pin pin, TIM_HandleTypeDef* timer, uint32_t channel_rising, uint32_t channel_falling) :
	pin(pin),
	timer(timer),
	channel_rising(channel_rising),
	channel_falling(channel_falling)
	{ }

optional<uint8_t> InputCapture::inscribe(Pin& pin){
 	if (not instances_data.contains(pin)) {
 		return nullopt;
 	}

	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	InputCapture::id_manager.pop_front();

 	Instance data = instances_data[pin];
	instances[id] = data;
	instances[id].id = id;
	return id;
}

void InputCapture::turn_on(uint8_t id){
	if (not instances.contains(id)) {
		return; //TODO: Error Handler
	}
	Instance instance = instances[id];
	HAL_TIM_IC_Start_IT(instance.timer, instance.channel_rising);
	HAL_TIM_IC_Start(instance.timer, instance.channel_falling);
}

void InputCapture::turn_off(uint8_t id){
	if (not instances.contains(id)) {
		return; //TODO: Error Handler
	}
	Instance instance = instances[id];
	HAL_TIM_IC_Stop_IT(instance.timer, instance.channel_rising);
	HAL_TIM_IC_Stop(instance.timer, instance.channel_falling);
}

optional<uint32_t> InputCapture::read_frequency(uint8_t id) {
	if (not instances.contains(id)) {
		return nullopt; //TODO: Error Handler
	}
	Instance instance = instances[id];
	return instance.frequency;
}

optional<uint8_t> InputCapture::read_duty_cycle(uint8_t id) {
	if (not instances.contains(id)) {
		return nullopt; //TODO: Error Handler
	}
	Instance instance = instances[id];
	return instance.duty_cycle;
}

InputCapture::Instance InputCapture::find_instance_by_channel(uint32_t channel) {
	for (auto id_instance: instances) {
		uint32_t& ch_rising = id_instance.second.channel_rising;
		uint32_t& ch_falling = id_instance.second.channel_falling;

		if(ch_rising == channel || ch_falling == channel) {
			return id_instance.second;
		}
	}
}

void HAL_TIM_InputCapture_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t& active_channel = channel_dict[htim->Channel];
	InputCapture::Instance instance = InputCapture::find_instance_by_channel(active_channel);

	uint32_t rising_value = HAL_TIM_ReadCapturedValue(htim, instance.channel_rising);
	if (rising_value != 0) {
		float ref_clock = HAL_RCC_GetPCLK1Freq()*2 / (instance.timer->Init.Prescaler+1);
		float falling_value = HAL_TIM_ReadCapturedValue(htim, instance.channel_falling);

		InputCapture::instances[instance.id].frequency = round(ref_clock / rising_value);
		InputCapture::instances[instance.id].duty_cycle = round((falling_value * 100) / rising_value);
	}
}


