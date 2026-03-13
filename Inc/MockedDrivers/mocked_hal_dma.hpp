#pragma once

#include "hal_wrapper.h"

#include <cstddef>
#include <cstdint>

namespace ST_LIB::MockedHAL {

enum class DMAOperation : uint8_t {
    Init = 0,
    StartIT,
    IRQHandler,
    ScheduleTransfer,
};

void dma_reset();
void dma_set_init_status(HAL_StatusTypeDef status);
void dma_set_start_status(HAL_StatusTypeDef status);
void dma_set_transfer_timing(uint64_t setup_time_ns, uint64_t per_byte_time_ns);

std::size_t dma_get_call_count(DMAOperation op);
DMA_HandleTypeDef* dma_get_last_init_handle();
DMA_HandleTypeDef* dma_get_last_start_handle();
DMA_HandleTypeDef* dma_get_last_irq_handle();
uint32_t dma_get_last_start_src();
uint32_t dma_get_last_start_dst();
uint32_t dma_get_last_start_length();
DMA_HandleTypeDef* dma_get_last_scheduled_handle();
uint64_t dma_get_last_schedule_request_time_ns();
uint64_t dma_get_last_schedule_completion_time_ns();
uint32_t dma_get_last_schedule_bytes();
uint64_t dma_schedule_transfer(DMA_HandleTypeDef* hdma, uint64_t request_time_ns, uint32_t bytes);

} // namespace ST_LIB::MockedHAL
