/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "ADC/ADC.hpp"

extern ADC_HandleTypeDef hadc3;

ADC::Instance::Instance(ADC_HandleTypeDef* adc, uint8_t rank, LowPowerTimer& timer, DMAStream& dma_stream) :
		adc(adc), rank(rank), timer(timer), dma_stream(dma_stream) {}

optional<uint8_t> ADC::inscribe(Pin pin) {
	if (not available_instances.contains(pin)) {
		return nullopt;
	}

	Pin::inscribe(pin, ANALOG);
	uint8_t id = id_manager.front();
	active_instances[id] = available_instances[pin];
	id_manager.pop_front();
	return id;
}

void ADC::start() {
	// TODO: ADC init
}

void ADC::turn_on(uint8_t id){
	if (not active_instances.contains(id)) {
		return;
	}

	DMAStream& dma_stream = active_instances[id].dma_stream;
	if (dma_stream.is_on) {
		return;
	}

	if (HAL_ADC_Start_DMA(active_instances[id].adc, (uint32_t*) dma_stream.buffer, dma_stream.buffer_length) != HAL_OK) {
		return; //TODO: Error handler
	}

	LowPowerTimer& timer = active_instances[id].timer;
	if (HAL_LPTIM_TimeOut_Start_IT(timer.handle, timer.period, timer.period / 2) != HAL_OK) {
		return; // TODO: Error handler
	}
	dma_stream.is_on = true;
}

optional<float> ADC::get_value(uint8_t id) {
	if (not active_instances.contains(id)) {
		return nullopt;
	}

	Instance& instance = active_instances[id];
	uint16_t& raw = instance.dma_stream.buffer[instance.rank-1];
	if(instance.adc == &hadc3) {
		return raw / MAX_12BIT * MAX_VOLTAGE;
	}
	else {
		return raw / MAX_16BIT * MAX_VOLTAGE;
	}
}
