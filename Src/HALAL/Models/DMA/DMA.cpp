/*
 * DMA.cpp
 *
 *  Created on: 10 dic. 2022
 *      Author: aleja
 */

#include "DMA/DMA.hpp"

void DMA::inscribe_stream(Stream dma_stream) {
	inscribed_streams.push_back(dma_stream);
}

void DMA::start() {
	for (Stream dma_stream : inscribed_streams) {
		HAL_NVIC_SetPriority( (IRQn_Type)dma_stream, 0, 0);
		HAL_NVIC_EnableIRQ( (IRQn_Type)dma_stream);
	}
}
