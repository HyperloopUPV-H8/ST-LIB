/*
 * DMA.cpp
 *
 *  Created on: 10 dic. 2022
 *      Author: aleja
 */

#include "DMA/DMA.hpp"

vector<DMA::Stream> DMA::available_streams = {
		DMA1Stream0, DMA1Stream1, DMA1Stream2, DMA1Stream3, DMA1Stream4, DMA1Stream5, DMA1Stream6, DMA2Stream0, DMA2Stream1, DMA2Stream2, DMA2Stream3, DMA2Stream4, DMA2Stream5, DMA2Stream6, DMA2Stream7,
};

void DMA::inscribe_stream() {
	if (available_streams.empty()) {
		return; //TODO: error handler
	}
	inscribed_streams.push_back(available_streams.back());
	available_streams.pop_back();
}
void DMA::inscribe_stream(Stream dma_stream) {
	if (std::find(available_streams.begin(), available_streams.end(), dma_stream) == available_streams.end()) {
		return; //TODO: error handler
	}
	inscribed_streams.push_back(dma_stream);
}

void DMA::start() {
	for (Stream dma_stream : inscribed_streams) {
		HAL_NVIC_SetPriority( (IRQn_Type)dma_stream, 0, 0);
		HAL_NVIC_EnableIRQ( (IRQn_Type)dma_stream);
	}
}
