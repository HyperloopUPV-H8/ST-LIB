/*
 * ADC.cpp
 *
 *  Created on: 20 oct. 2022
 *      Author: alejandro
 */

#include "HALALMock/Services/ADC/ADC.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#if defined(HAL_ADC_MODULE_ENABLED) && defined(HAL_LPTIM_MODULE_ENABLED)

extern ADC_HandleTypeDef hadc3;

uint8_t ADC::id_counter = 0;
unordered_map<uint8_t, ADC::Instance> ADC::active_instances = {};


ADC::InitData::InitData(ADC_TypeDef* adc, uint32_t resolution, uint32_t external_trigger, vector<uint32_t>& channels, DMA::Stream dma_stream, string name) :
		adc(adc), resolution(resolution), external_trigger(external_trigger), channels(channels), dma_stream(dma_stream), name(name) {
			switch (resolution) {
				case 0x00000000UL: // 16 bits
					emulated_resolution = ADCResolution::ADC_RES_16BITS;
					break;
				case 0x00000004UL: // 14 bits
					emulated_resolution = ADCResolution::ADC_RES_14BITS;
					break;
				case 0x00000008UL: // 12 bits
					emulated_resolution = ADCResolution::ADC_RES_12BITS;
					break;
				case 0x0000000CUL: // 10 bits
					emulated_resolution = ADCResolution::ADC_RES_10BITS;
					break;
			}
		}

ADC::Peripheral::Peripheral(ADC_HandleTypeDef* handle, LowPowerTimer& timer, InitData& init_data) :
	handle(handle), timer(timer), init_data(init_data) {
	// dma_data_buffer = (uint16_t*)MPUManager::allocate_non_cached_memory(2*ADC_BUF_LEN); Comented because of the mock
}

bool ADC::Peripheral::is_registered() {
	return init_data.channels.size();
}

ADC::Instance::Instance(ADC::Peripheral* peripheral, uint32_t channel) :
		peripheral(peripheral), channel(channel) {}

uint8_t ADC::inscribe(Pin pin) {
	if (not available_instances.contains(pin)) {
		ErrorHandler("Pin %s is already used or isn t available for ADC usage", pin.to_string().c_str());
		return 0;
	}

	// Emulated pin inscribe
	EmulatedPin& emulated_pin = SharedMemory::get_pin(pin);
	if (emulated_pin.type != PinType::NOT_USED) {
		//ErrorHandler("Pin %s is not available for ADC usage", pin.to_string().c_str());
		printf("Pin %s is not available for ADC usage, is already using as %s", pin.to_string().c_str(), emulated_pin.type);
		return 0;
	}

	Pin::inscribe(pin, OperationMode::ANALOG);
	active_instances[id_counter] = available_instances[pin];

	InitData& init_data = active_instances[id_counter].peripheral->init_data;
	DMA::inscribe_stream(init_data.dma_stream);
	active_instances[id_counter].rank = init_data.channels.size();
	init_data.channels.push_back(active_instances[id_counter].channel);
	return id_counter++;
}

void ADC::start() {
	for(Peripheral& peripheral : peripherals) {
		if (peripheral.is_registered()) {
			ADC::init(peripheral);
		}
	}

	// Storing emulated pins with their corresponding instance
	for (auto& [pin, instance] : available_instances) {
		EmulatedPin& emulated_pin = SharedMemory::get_pin(pin);
		available_emulated_instances[instance] = emulated_pin;
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
/*
	uint32_t buffer_length = peripheral->init_data.channels.size();
	if (HAL_ADC_Start_DMA(peripheral->handle, (uint32_t*) peripheral->dma_data_buffer, buffer_length) != HAL_OK) {
		ErrorHandler("DMA - %d - of ADC - %d - did not start correctly", peripheral->init_data.dma_stream, id);
		return;
	}

	LowPowerTimer& timer = peripheral->timer;
	if (HAL_LPTIM_TimeOut_Start_IT(&timer.handle, timer.period, timer.period / 2) != HAL_OK) {
		ErrorHandler("LPTIM - %d - of ADC - %d - did not start correctly", timer.name, peripheral->handle);
		return;
	}
*/ // Commented out because of the mock

	peripheral->is_on = true;
}

float ADC::get_value(uint8_t id) {
	Instance& instance = active_instances[id];

	EmulatedPin& emulated_pin = available_emulated_instances[instance];
	if (emulated_pin.type != PinType::ADC) {
		printf("Pin %s is not configured to be used for ADC usage. WARNING: it means that this pin \
				has been reconfigured to be another peripheral WHILE this pin was already configured to be an ADC", emulated_pin);
		return 0;
	}
	ADCResolution resolution = instance.peripheral->init_data.emulated_resolution;
	uint16_t raw = emulated_pin.PinData.ADC.value;
	switch (resolution) {
		case ADCResolution::ADC_RES_16BITS:
			return raw / MAX_16BIT * ADC_MAX_VOLTAGE;
		case ADCResolution::ADC_RES_14BITS:
			return raw / MAX_14BIT * ADC_MAX_VOLTAGE;
		case ADCResolution::ADC_RES_12BITS:
			return raw / MAX_12BIT * ADC_MAX_VOLTAGE;
		case ADCResolution::ADC_RES_10BITS:
			return raw / MAX_10BIT * ADC_MAX_VOLTAGE;
	}
}

uint16_t ADC::get_int_value(uint8_t id) {
	Instance& instance = active_instances[id];

	EmulatedPin& emulated_pin = available_emulated_instances[instance];
	if (emulated_pin.type != PinType::ADC) {
		printf("Pin %s is not configured to be used for ADC usage. WARNING: it means that this pin \
				has been reconfigured to be another peripheral WHILE this pin was already configured to be an ADC", emulated_pin);
		return 0;
	}

	uint16_t raw = emulated_pin.PinData.ADC.value;

	ADCResolution resolution = instance.peripheral->init_data.emulated_resolution;
	uint16_t raw = emulated_pin.PinData.ADC.value;
	switch (resolution) {
		case ADCResolution::ADC_RES_16BITS:
			return raw;
		case ADCResolution::ADC_RES_14BITS:
			return raw << 2;
		case ADCResolution::ADC_RES_12BITS:
			return raw << 4;
		case ADCResolution::ADC_RES_10BITS:
			return raw << 6;
	}
}

uint16_t* ADC::get_value_pointer(uint8_t id) {
	Instance& instance = active_instances[id];
	//return &instance.peripheral->dma_data_buffer[instance.rank]; Commented out because of the mock
	return &available_emulated_instances[instance].PinData.ADC.value;
}

void ADC::init(Peripheral& peripheral) {
	/*
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
	if (adc_handle.Instance == ADC3) {
		adc_handle.Init.DMAContinuousRequests = ENABLE;
	} else {
		adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
	}
	adc_handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	adc_handle.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&adc_handle) != HAL_OK) {
		ErrorHandler("ADC  - %d - did not start correctly", init_data.name);
		return;
	}

	multimode.Mode = ADC_MODE_INDEPENDENT;
	if(adc_handle.Instance == ADC1){
		if (HAL_ADCEx_MultiModeConfigChannel(&adc_handle, &multimode) != HAL_OK) {
			ErrorHandler("ADC MultiModeConfigChannel - %d - did not start correctly", init_data.name);
			return;
		}
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
		ErrorHandler("ADC ConfigChannel - %d - did not start correctly", init_data.name);
	  }
	  counter++;
	}

	  peripheral.timer.init();
	*/ // Commented out because of the mock
}
#endif
