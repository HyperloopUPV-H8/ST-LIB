/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "ADC/ADC.hpp"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern LPTIM_HandleTypeDef hlptim1;
extern LPTIM_HandleTypeDef hlptim2;
extern LPTIM_HandleTypeDef hlptim3;

uint16_t adc_buf1[ADC_BUF1_LEN];
uint16_t adc_buf2[ADC_BUF2_LEN];
uint16_t adc_buf3[ADC_BUF3_LEN];

forward_list<uint8_t> ADC::id_manager = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
map<uint8_t, ADC::Instance> ADC::active_instances = {};

dma_buffer dma_buffer1 = dma_buffer(adc_buf1, ADC_BUF1_LEN);
low_power_timer lptim1 = low_power_timer(&hlptim1, LPTIM1_PERIOD);
dma_buffer dma_buffer2 = dma_buffer(adc_buf2, ADC_BUF2_LEN);
low_power_timer lptim2 = low_power_timer(&hlptim2, LPTIM2_PERIOD);
dma_buffer dma_buffer3 = dma_buffer(adc_buf3, ADC_BUF3_LEN);
low_power_timer lptim3 = low_power_timer(&hlptim3, LPTIM3_PERIOD);

map<ADC_HandleTypeDef*, adc_data> ADC::peripheral_data = {
		{&hadc1, adc_data(dma_buffer1, lptim1)},
		{&hadc2, adc_data(dma_buffer2, lptim2)},
		{&hadc3, adc_data(dma_buffer3, lptim3)}
};

//map<ADC_HandleTypeDef*, adc_data> ADC::peripheral_data = {
//		{&hadc1, { .buffer = { .data = adc_buf1, .length = ADC_BUF1_LEN }, .timer = { .handle = &hlptim1, .period = LPTIM1_PERIOD} }},
//		{&hadc2, { .buffer = { .data = adc_buf2, .length = ADC_BUF2_LEN }, .timer = {.handle = &hlptim2, .period = LPTIM2_PERIOD} }},
//		{&hadc3, { .buffer = { .data = adc_buf3, .length = ADC_BUF3_LEN }, .timer = {.handle = &hlptim3, .period = LPTIM3_PERIOD} }}
//};

ADC::Instance::Instance(ADC_HandleTypeDef* adc, uint8_t rank) : adc(adc), rank(rank){}

map<Pin, ADC::Instance> ADC::available_instances = {
		{PF11, Instance(&hadc1, 1)},
		{PA6, Instance(&hadc1, 2)},
		{PC4, Instance(&hadc1, 3)},
		{PB1, Instance(&hadc1, 4)},
		{PF13, Instance(&hadc2, 1)},
		{PF14, Instance(&hadc2, 2)},
		{PC2, Instance(&hadc3, 1)},
		{PC3, Instance(&hadc3, 2)}
};

optional<uint8_t> ADC::inscribe(Pin pin) {
	if (not available_instances.contains(pin)) {
		return nullopt;
	}

	Pin::register_pin(pin, ANALOG);
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

	ADC_HandleTypeDef* adc = active_instances[id].adc;
	if (peripheral_data[adc].is_on) {
		return;
	}

	dma_buffer& buf = peripheral_data[adc].buffer;
	if (HAL_ADC_Start_DMA(adc, (uint32_t*) buf.data, buf.length) != HAL_OK) {
		return; // TODO Error handling
	}

	low_power_timer& timer = peripheral_data[adc].timer;
	if (HAL_LPTIM_TimeOut_Start_IT(timer.handle, timer.period, timer.period / 2) != HAL_OK) {
		return; // TODO Error handling
	}
	peripheral_data[adc].is_on = true;
}

void ADC::turn_off(uint8_t id) {
	if (not active_instances.contains(id)) {
		return;
	}

	ADC_HandleTypeDef* adc = active_instances[id].adc;
	if (not peripheral_data[adc].is_on) {
		return;
	}
	if (HAL_ADC_Stop_DMA(active_instances[id].adc) != HAL_OK) { } // TODO Error handling
	peripheral_data[adc].is_on = false;
}

optional<float> ADC::get_value(uint8_t id) {
	if (not active_instances.contains(id)) {
		return nullopt;
	}

	Instance& instance = active_instances[id];
	uint16_t& raw = peripheral_data[instance.adc].buffer.data[instance.rank-1];
	if(instance.adc == &hadc3) {
		return raw / MAX_12BIT * MAX_VOLTAGE;
	}
	else {
		return raw / MAX_16BIT * MAX_VOLTAGE;
	}
}
