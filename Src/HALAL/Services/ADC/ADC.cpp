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

uint16_t adc_buf1[ADC_BUF1_LEN];
uint16_t adc_buf2[ADC_BUF2_LEN];
uint16_t adc_buf3[ADC_BUF3_LEN];

uint16_t adc_read1[ADC_BUF1_LEN];
uint16_t adc_read2[ADC_BUF2_LEN];
uint16_t adc_read3[ADC_BUF3_LEN];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc == &hadc1){
		copy(adc_buf1 + ADC_BUF1_LEN / 2, adc_buf1 + ADC_BUF1_LEN, adc_read1 + ADC_BUF1_LEN / 2);
	}
	else if (hadc == &hadc2){
		copy(adc_buf2 + ADC_BUF2_LEN / 2, adc_buf2 + ADC_BUF2_LEN, adc_read2 + ADC_BUF2_LEN / 2);
	}
	else if (hadc == &hadc3){
		copy(adc_buf3 + ADC_BUF3_LEN / 2, adc_buf3 + ADC_BUF3_LEN, adc_read3 + ADC_BUF3_LEN / 2);
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc == &hadc1){
		copy(adc_buf1, adc_buf1 + ADC_BUF1_LEN / 2, adc_read1);
	}
	else if (hadc == &hadc2){
		copy(adc_buf2, adc_buf2 + ADC_BUF2_LEN / 2, adc_read2);
	}
	else if (hadc == &hadc3){
		copy(adc_buf3, adc_buf3 + ADC_BUF3_LEN / 2, adc_read3);
	}
}

optional<uint8_t> ADC::inscribe(Pin pin) {
	if (not pin_adc_map.contains(pin)) {
		return nullopt; //TODO: error handler
	}

	Pin::inscribe(pin, ALTERNATIVE);
	uint8_t id = id_manager.front();
	ADC::service_ids[id] = pin;
	ADC::id_manager.pop_front();
	return id;
}

void ADC::turn_on(uint8_t id){
	if (not service_ids.contains(id)) {
		return; //TODO: error handler
	}
	Pin pin = service_ids[id];
	ADCchannel adc_channel = pin_adc_map[pin];


	HAL_StatusTypeDef status;
	if (adc_channel.adc == &hadc1){
		status = HAL_ADC_Start_DMA(adc_channel.adc, (uint32_t*)adc_buf1, ADC_BUF1_LEN);
	}
	else if (adc_channel.adc == &hadc2){
		status = HAL_ADC_Start_DMA(adc_channel.adc, (uint32_t*)adc_buf2, ADC_BUF2_LEN);
	}
	else if (adc_channel.adc == &hadc3){
		status = HAL_ADC_Start_DMA(adc_channel.adc, (uint32_t*)adc_buf3, ADC_BUF3_LEN);
	}

	if (status != HAL_OK) { }//TODO: Error handler
}
