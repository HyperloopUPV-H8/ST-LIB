/*
 * DMA.cpp
 *
 *  Created on: 10 dic. 2022
 *      Author: aleja
 */

#include "DMA/DMA.hpp"

#include "ErrorHandler/ErrorHandler.hpp"

vector<DMA::Stream> DMA::inscribed_streams = {};

void DMA::inscribe_stream() {
    if (available_streams.empty()) {
        ErrorHandler("There are not any DMA streams available");
        return;
    }
    inscribed_streams.push_back(available_streams.back());
    available_streams.pop_back();
}

void DMA::inscribe_stream(Stream dma_stream) {
    if (std::find(available_streams.begin(), available_streams.end(),
                  dma_stream) == available_streams.end()) {
        ErrorHandler("The DMA stream %d is not available", dma_stream);
        return;
    }

    inscribed_streams.push_back(dma_stream);
}

void DMA::start() {
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
    for (Stream dma_stream : inscribed_streams) {
        HAL_NVIC_SetPriority((IRQn_Type)dma_stream, 0, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)dma_stream);
    }
}
