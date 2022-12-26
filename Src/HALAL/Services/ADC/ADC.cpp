/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "ADC/ADC.hpp"

extern ADC_HandleTypeDef hadc3;

uint8_t ADC::id_counter = 0;
map<uint8_t, ADC::Instance> ADC::active_instances = {};

ADC::InitData::InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger, vector<uint32_t>& channels) :
		adc(adc), resolution(resolution), external_trigger(external_trigger), channels(channels) {}

ADC::Peripheral::Peripheral(ADC_HandleTypeDef* handle, uint16_t* dma_stream, LowPowerTimer& timer, InitData& init_data) :
	handle(handle), dma_stream(dma_stream), timer(timer), init_data(init_data) {}

bool ADC::Peripheral::is_registered() {
	return init_data.channels.size();
}

ADC::Instance::Instance(ADC::Peripheral* peripheral, uint32_t channel) :
		peripheral(peripheral), channel(channel) {}

optional<uint8_t> ADC::inscribe(Pin pin) {
	if (not available_instances.contains(pin)) {
		return nullopt;
	}

	Pin::inscribe(pin, ANALOG);
	active_instances[id_counter] = available_instances[pin];

	Instance& instance = active_instances[id_counter];
	load_init_data(instance);

	return id_counter++;
}

void ADC::start() {
	for(Peripheral& peripheral : peripherals) {
		if (peripheral.is_registered()) {
			ADC::init(peripheral);
		}
	}
}

void ADC::turn_on(uint8_t id){
	if (not active_instances.contains(id)) {
		return;
	}

	Peripheral* peripheral = active_instances[id].peripheral;
	if (peripheral->is_on) {
		return;
	}

	uint32_t buffer_length = peripheral->init_data.channels.size();
	if (HAL_ADC_Start_DMA(peripheral->handle, (uint32_t*) peripheral->dma_stream, buffer_length) != HAL_OK) {
		return; //TODO: Error handler
	}

	LowPowerTimer& timer = peripheral->timer;
	if (HAL_LPTIM_TimeOut_Start_IT(timer.handle, timer.period, timer.period / 2) != HAL_OK) {
		return; //TODO: Error handler
	}

	peripheral->is_on = true;
}

optional<float> ADC::get_value(uint8_t id) {
	if (not active_instances.contains(id)) {
		return nullopt;
	}

	Instance& instance = active_instances[id];
	uint16_t raw = instance.peripheral->dma_stream[instance.rank];
	if(instance.peripheral->handle == &hadc3) {
		return raw / MAX_12BIT * MAX_VOLTAGE;
	}
	else {
		return raw / MAX_16BIT * MAX_VOLTAGE;
	}

	return nullopt;
}


/************************************************
 *              PRIVATE FUNCTIONS
 ***********************************************/

void ADC::init(Peripheral& peripheral) {
	  ADC_MultiModeTypeDef multimode = {0};
	  ADC_ChannelConfTypeDef sConfig = {0};
	  ADC_HandleTypeDef& adc_handle = *peripheral.handle;
	  InitData init_data = peripheral.init_data;

	  adc_handle.Instance = init_data.adc;
	  adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	  adc_handle.Init.Resolution = init_data.resolution;
	  adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;
	  adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	  adc_handle.Init.LowPowerAutoWait = DISABLE;
	  adc_handle.Init.ContinuousConvMode = DISABLE;
	  adc_handle.Init.NbrOfConversion = init_data.channels.size();
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


	  uint8_t counter = 0;
	  for(uint32_t channel : peripheral.init_data.channels) {
		  sConfig.Channel = channel;
	  	  sConfig.Rank = ranks[counter];
	  	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  	  sConfig.Offset = 0;
	  	  sConfig.OffsetSignedSaturation = DISABLE;
	  	  if (HAL_ADC_ConfigChannel(&adc_handle, &sConfig) != HAL_OK) {
	  		  //TODO: Error handler
	  	  }
	  	  counter++;
	  }
}

void ADC::load_init_data(Instance instance) {
	InitData& init_data = instance.peripheral->init_data;
	instance.rank = init_data.channels.size();
	init_data.channels.push_back(instance.channel);
}
