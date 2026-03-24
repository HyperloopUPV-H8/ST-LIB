#include "MockedDrivers/mocked_hal_dma.hpp"

#include <algorithm>
#include <array>
#include <unordered_map>

namespace {

constexpr std::size_t kDmaOperationCount =
    static_cast<std::size_t>(ST_LIB::MockedHAL::DMAOperation::ScheduleTransfer) + 1U;

struct DMAState {
    HAL_StatusTypeDef init_status = HAL_OK;
    HAL_StatusTypeDef start_status = HAL_OK;
    std::array<std::size_t, kDmaOperationCount> calls{};
    DMA_HandleTypeDef* last_init_handle = nullptr;
    DMA_HandleTypeDef* last_start_handle = nullptr;
    DMA_HandleTypeDef* last_irq_handle = nullptr;
    uint32_t last_start_src = 0;
    uint32_t last_start_dst = 0;
    uint32_t last_start_length = 0;
    uint64_t transfer_setup_time_ns = 0;
    uint64_t transfer_per_byte_time_ns = 0;
    uint64_t bus_available_time_ns = 0;
    DMA_HandleTypeDef* last_scheduled_handle = nullptr;
    uint64_t last_schedule_request_time_ns = 0;
    uint64_t last_schedule_completion_time_ns = 0;
    uint32_t last_schedule_bytes = 0;
    std::unordered_map<const DMA_HandleTypeDef*, uint64_t> stream_available_time_ns{};
};

DMAState g_state{};

} // namespace

namespace ST_LIB::MockedHAL {

void dma_reset() { g_state = {}; }

void dma_set_init_status(HAL_StatusTypeDef status) { g_state.init_status = status; }

void dma_set_start_status(HAL_StatusTypeDef status) { g_state.start_status = status; }

void dma_set_transfer_timing(uint64_t setup_time_ns, uint64_t per_byte_time_ns) {
    g_state.transfer_setup_time_ns = setup_time_ns;
    g_state.transfer_per_byte_time_ns = per_byte_time_ns;
}

std::size_t dma_get_call_count(DMAOperation op) {
    return g_state.calls[static_cast<std::size_t>(op)];
}

DMA_HandleTypeDef* dma_get_last_init_handle() { return g_state.last_init_handle; }

DMA_HandleTypeDef* dma_get_last_start_handle() { return g_state.last_start_handle; }

DMA_HandleTypeDef* dma_get_last_irq_handle() { return g_state.last_irq_handle; }

uint32_t dma_get_last_start_src() { return g_state.last_start_src; }

uint32_t dma_get_last_start_dst() { return g_state.last_start_dst; }

uint32_t dma_get_last_start_length() { return g_state.last_start_length; }

DMA_HandleTypeDef* dma_get_last_scheduled_handle() { return g_state.last_scheduled_handle; }

uint64_t dma_get_last_schedule_request_time_ns() { return g_state.last_schedule_request_time_ns; }

uint64_t dma_get_last_schedule_completion_time_ns() {
    return g_state.last_schedule_completion_time_ns;
}

uint32_t dma_get_last_schedule_bytes() { return g_state.last_schedule_bytes; }

uint64_t dma_schedule_transfer(DMA_HandleTypeDef* hdma, uint64_t request_time_ns, uint32_t bytes) {
    g_state.calls[static_cast<std::size_t>(ST_LIB::MockedHAL::DMAOperation::ScheduleTransfer)]++;
    g_state.last_scheduled_handle = hdma;
    g_state.last_schedule_request_time_ns = request_time_ns;
    g_state.last_schedule_bytes = bytes;

    if (hdma == nullptr) {
        g_state.last_schedule_completion_time_ns = request_time_ns;
        return request_time_ns;
    }

    // The timing model is intentionally simple: one shared DMA bus plus per-stream serialization.
    auto& stream_available = g_state.stream_available_time_ns[hdma];
    const uint64_t start_time =
        std::max(request_time_ns, std::max(stream_available, g_state.bus_available_time_ns));
    const uint64_t completion_time =
        start_time + g_state.transfer_setup_time_ns + g_state.transfer_per_byte_time_ns * bytes;

    stream_available = completion_time;
    g_state.bus_available_time_ns = completion_time;
    g_state.last_schedule_completion_time_ns = completion_time;
    return completion_time;
}

} // namespace ST_LIB::MockedHAL

extern "C" HAL_StatusTypeDef MockedHAL_DMA_Init_Impl(DMA_HandleTypeDef* hdma) {
    g_state.calls[static_cast<std::size_t>(ST_LIB::MockedHAL::DMAOperation::Init)]++;
    g_state.last_init_handle = hdma;
    if (hdma == nullptr) {
        return HAL_ERROR;
    }
    return g_state.init_status;
}

extern "C" HAL_StatusTypeDef MockedHAL_DMA_Start_IT_Impl(
    DMA_HandleTypeDef* hdma,
    uint32_t SrcAddress,
    uint32_t DstAddress,
    uint32_t DataLength
) {
    g_state.calls[static_cast<std::size_t>(ST_LIB::MockedHAL::DMAOperation::StartIT)]++;
    g_state.last_start_handle = hdma;
    g_state.last_start_src = SrcAddress;
    g_state.last_start_dst = DstAddress;
    g_state.last_start_length = DataLength;
    if (hdma == nullptr) {
        return HAL_ERROR;
    }
    return g_state.start_status;
}

extern "C" void MockedHAL_DMA_IRQHandler_Impl(DMA_HandleTypeDef* hdma) {
    g_state.calls[static_cast<std::size_t>(ST_LIB::MockedHAL::DMAOperation::IRQHandler)]++;
    g_state.last_irq_handle = hdma;
}
