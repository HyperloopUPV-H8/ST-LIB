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

ADC::Instance::Instance(ADC_HandleTypeDef* adc, uint8_t rank, low_power_timer* timer, dma_buffer* buffer) :
		adc(adc), rank(rank), timer(timer), buffer(buffer) {}

dma_buffer dma_buffer1 = dma_buffer(adc_buf1, ADC_BUF1_LEN);
low_power_timer lptim1 = low_power_timer(&hlptim1, LPTIM1_PERIOD);
dma_buffer dma_buffer2 = dma_buffer(adc_buf2, ADC_BUF2_LEN);
low_power_timer lptim2 = low_power_timer(&hlptim2, LPTIM2_PERIOD);
dma_buffer dma_buffer3 = dma_buffer(adc_buf3, ADC_BUF3_LEN);
low_power_timer lptim3 = low_power_timer(&hlptim3, LPTIM3_PERIOD);

map<Pin, ADC::Instance> ADC::available_instances = {
		{PF11, Instance(&hadc1, 1, &lptim1, &dma_buffer1)},
		{PA6, Instance(&hadc1, 2, &lptim1, &dma_buffer1)},
		{PC4, Instance(&hadc1, 3, &lptim1, &dma_buffer1)},
		{PB1, Instance(&hadc1, 4, &lptim1, &dma_buffer1)},
		{PF13, Instance(&hadc2, 1, &lptim2, &dma_buffer2)},
		{PF14, Instance(&hadc2, 2, &lptim2, &dma_buffer2)},
		{PC2, Instance(&hadc3, 1, &lptim3, &dma_buffer3)},
		{PC3, Instance(&hadc3, 2, &lptim3, &dma_buffer3)}
};

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
		{&hadc2, InitData(
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

	Pin::register_pin(pin, ANALOG);
	uint8_t id = id_manager.front();
	active_instances[id] = available_instances[pin];
	id_manager.pop_front();
	return id;
}

void ADC::start() {
	ADC::init(hadc1);
	ADC::init(hadc2);
	ADC::init(hadc3);
}

void ADC::turn_on(uint8_t id){
	if (not active_instances.contains(id)) {
		return;
	}

	dma_buffer* buffer = active_instances[id].buffer;
	if (buffer->is_on) {
		return;
	}

	if (HAL_ADC_Start_DMA(active_instances[id].adc, (uint32_t*) buffer->data, buffer->length) != HAL_OK) {
		return; // TODO: Error handling
	}

	low_power_timer* timer = active_instances[id].timer;
	if (HAL_LPTIM_TimeOut_Start_IT(timer->handle, timer->period, timer->period / 2) != HAL_OK) {
		return; // TODO: Error handling
	}
	buffer->is_on = true;
}

optional<float> ADC::get_value(uint8_t id) {
	if (not active_instances.contains(id)) {
		return nullopt;
	}

	Instance& instance = active_instances[id];
	uint16_t& raw = instance.buffer->data[instance.rank-1];
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
	    // TODO: Error handler
	  }

	  multimode.Mode = ADC_MODE_INDEPENDENT;
	  if (HAL_ADCEx_MultiModeConfigChannel(&adc_handle, &multimode) != HAL_OK) {
	    // TODO: Error handler
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
	  		  // TODO: Error handler
	  	  }
	  }
}
