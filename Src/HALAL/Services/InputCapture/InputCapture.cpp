/*
 * InputCapture.cpp
 *
 *  Created on: 30 oct. 2022
 *      Author: alejandro
 */
#include "InputCapture/InputCapture.hpp"

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
		return; // Error Handler
	}
	Instance instance = instances[id];
	HAL_TIM_IC_Start_IT(instance.timer, instance.channel_rising);
	HAL_TIM_IC_Start(instance.timer, instance.channel_falling);
}

void InputCapture::turn_off(uint8_t id){
	if (not instances.contains(id)) {
		return; // Error Handler
	}
	Instance instance = instances[id];
	HAL_TIM_IC_Stop_IT(instance.timer, instance.channel_rising);
	HAL_TIM_IC_Stop(instance.timer, instance.channel_falling);
}

optional<uint32_t> InputCapture::read_frequency(uint8_t id) {
	if (not instances.contains(id)) {
		return nullopt; // Error Handler
	}
	Instance instance = instances[id];
	return instance.frequency;
}

optional<uint8_t> InputCapture::read_duty_cycle(uint8_t id) {
	if (not instances.contains(id)) {
		return nullopt; // Error Handler
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


