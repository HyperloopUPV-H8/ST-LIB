/*
 * DMA.hpp
 *
 *  Created on: 10 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "stm32h7xx_hal.h"
#include "MPUManager/MPUManager.hpp"


class DMA {
public:
	enum NVICnumber : uint8_t {
		DMA1Stream0 = 11,
		DMA1Stream1 = 12,
		DMA1Stream2 = 13,
		DMA1Stream3 = 14,
		DMA1Stream4 = 15,
		DMA1Stream5 = 16,
		DMA1Stream6 = 17,
		DMA2Stream0 = 56,
		DMA2Stream1 = 57,
		DMA2Stream2 = 58,
		DMA2Stream3 = 59,
		DMA2Stream4 = 60,
		DMA2Stream5 = 68,
		DMA2Stream6 = 69,
		DMA2Stream7 = 70,
	};

	template <class HandleType>
	struct InitData {
		DMA_Stream_TypeDef* instance;
		NVICnumber nvic_number;
		uint8_t adc_request;
		uint8_t direction;
		uint8_t mode;
		HandleType linked_handle;
		DMA_HandleTypeDef* handle_pointer;

		InitData() = default;
		InitData(DMA_Stream_TypeDef* instance, uint8_t adc_request, uint8_t direction, uint8_t mode, NVICnumber nvic_number=0) :
			instance(instance), adc_request(adc_request), direction(direction), mode(mode), nvic_number(nvic_number) {};
	};

	class Stream {
	public:
		InitData init_data;
		DMA_HandleTypeDef handle;

		Stream() = default;
		Stream(DMA_HandleTypeDef handle, InitData init_data);

	};

	static void inscribe_stream();
	static void inscribe_stream(Stream dma_stream);
	static void start();

private:
	static vector<Stream> available_streams;
	static vector<Stream> inscribed_streams;
};

template <class HandleType>
void DMA::activate_IT() {
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	for (Stream dma_stream : inscribed_streams) {
		NVICnumber& nvic_number = dma_stream.init_data.nvic_number;
		if (nvic_number) {
			HAL_NVIC_SetPriority((IRQn_Type)dma_stream, 0, 0);
			HAL_NVIC_EnableIRQ((IRQn_Type)dma_stream);
		}

		DMA_HandleTypeDef& handle = dma_stream.handle;
		InitData& i = dma_stream.init_data;
	    handle.Instance = i.instance;
	    handle.Init.Request = i.adc_request;
	    handle.Init.Direction = i.direction;
	    handle.Init.PeriphInc = DMA_PINC_DISABLE;
	    handle.Init.MemInc = DMA_MINC_ENABLE;
	    handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	    handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	    handle.Init.Mode = dma_stream,init_data.mode;
	    handle.Init.Priority = DMA_PRIORITY_LOW;
	    handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	    if (HAL_DMA_Init(&handle) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    __HAL_LINKDMA(i.linked_handle, i.handle_pointer, handle);
	}
}
