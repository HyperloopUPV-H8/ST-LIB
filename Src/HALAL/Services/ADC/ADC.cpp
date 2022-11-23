/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "ADC/ADC.hpp"

extern ADC_HandleTypeDef hadc3;

ADC::Instance::Instance(ADC_HandleTypeDef* adc, uint8_t rank, low_power_timer& timer, dma_buffer& buffer) :
		adc(adc), rank(rank), timer(timer), buffer(buffer) {}

<<<<<<< HEAD
ADC::InitData::InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger, vector<ChannelRank> channel_rank_vector) :
		adc(adc), resolution(resolution), external_trigger(external_trigger), channel_rank_vector(channel_rank_vector) {}

map<ADC_HandleTypeDef*, ADC::InitData> ADC::init_data_map = {
		{&hadc1, InitData(
			ADC1,
			ADC_RESOLUTION_16B,
			ADC_EXTERNALTRIG_LPTIM1_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)},
		{&hadc2, InitData(
			ADC2,
			ADC_RESOLUTION_16B,
			ADC_EXTERNALTRIG_LPTIM2_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)},
		{&hadc3, InitData(
			ADC3,
			ADC_RESOLUTION_12B,
			ADC_EXTERNALTRIG_LPTIM3_OUT,
			{{ADC_CHANNEL_2, ADC_REGULAR_RANK_1}, {ADC_CHANNEL_3, ADC_REGULAR_RANK_2}}
		)}
};

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
	for(auto adc_initinfo : init_data_map) {
		ADC::init(*adc_initinfo.first);
	}
}

void ADC::turn_on(uint8_t id){
	if (not active_instances.contains(id)) {
		return;
	}

	dma_buffer& buffer = active_instances[id].buffer;
	if (buffer.is_on) {
		return;
	}

	if (HAL_ADC_Start_DMA(active_instances[id].adc, (uint32_t*) buffer->data, buffer->length) != HAL_OK) {
		return; //TODO: Error handler
	}

	low_power_timer* timer = active_instances[id].timer;
	if (HAL_LPTIM_TimeOut_Start_IT(timer->handle, timer->period, timer->period / 2) != HAL_OK) {
		return; //TODO: Error handler
	}
	buffer.is_on = true;
}

optional<float> ADC::get_value(uint8_t id) {
	if (not active_instances.contains(id)) {
		return nullopt;
	}

	Instance& instance = active_instances[id];
	uint16_t& raw = instance.buffer.data[instance.rank-1];
	if(instance.adc == &hadc3) {
		return raw / MAX_12BIT * MAX_VOLTAGE;
	}
	else {
		return raw / MAX_16BIT * MAX_VOLTAGE;
	}
}

void ADC::init(ADC_HandleTypeDef& adc_handle) {


	  ADC_MultiModeTypeDef multimode = {0};
	  ADC_ChannelConfTypeDef sConfig = {0};
	  ADC::InitData init_data = init_data_map[&adc_handle];

	  adc_handle.Instance = init_data.adc;
	  adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	  adc_handle.Init.Resolution = init_data.resolution;
	  adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;
	  adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	  adc_handle.Init.LowPowerAutoWait = DISABLE;
	  adc_handle.Init.ContinuousConvMode = DISABLE;
	  adc_handle.Init.NbrOfConversion = init_data.channel_rank_vector.size();
	  adc_handle.Init.DiscontinuousConvMode = DISABLE;
	  adc_handle.Init.ExternalTrigConv = init_data.external_trigger;
	  adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	  adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
	  adc_handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	  adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	  adc_handle.Init.OversamplingMode = DISABLE;
	  if (HAL_ADC_Init(&adc_handle) != HAL_OK) {
	    //TODO: Error handler
	  }

	  multimode.Mode = ADC_MODE_INDEPENDENT;
	  if (HAL_ADCEx_MultiModeConfigChannel(&adc_handle, &multimode) != HAL_OK) {
	    //TODO: Error handler
	  }


	  for(auto channel_rank : init_data.channel_rank_vector) {
		  sConfig.Channel = channel_rank.channel;
	  	  sConfig.Rank = channel_rank.rank;
	  	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  	  sConfig.Offset = 0;
	  	  sConfig.OffsetSignedSaturation = DISABLE;
	  	  if (HAL_ADC_ConfigChannel(&adc_handle, &sConfig) != HAL_OK) {
	  		  //TODO: Error handler
	  	  }
	  }
}
