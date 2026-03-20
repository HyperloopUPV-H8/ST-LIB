#pragma once

#include "hal_wrapper.h"

#include <functional>

namespace ST_LIB::MockedHAL {

enum class ADCOperation : uint8_t {
    Init = 0,
    ConfigChannel,
    Start,
    PollForConversion,
    GetValue,
    Stop,
    StartDMA,
    StopDMA,
};

using ADCSignalGenerator = std::function<uint32_t(uint64_t)>;

void adc_reset();

void adc_set_channel_raw(ADC_TypeDef* adc, uint32_t channel, uint32_t raw_value);
void adc_set_channel_generator(ADC_TypeDef* adc, uint32_t channel, ADCSignalGenerator generator);

void adc_set_poll_timeout(ADC_TypeDef* adc, bool enabled);
void adc_enable_timed_dma(ADC_TypeDef* adc, bool enabled);
void adc_set_kernel_clock_hz(ADC_TypeDef* adc, uint64_t kernel_clock_hz);
void adc_advance_time_ns(uint64_t delta_ns);
uint64_t adc_get_time_ns();

uint32_t adc_get_last_channel(ADC_TypeDef* adc);

bool adc_is_running(ADC_TypeDef* adc);

bool adc_is_dma_running(ADC_TypeDef* adc);

std::size_t adc_get_call_count(ADCOperation op);

uint32_t adc_get_dma_length(ADC_TypeDef* adc);

uint32_t adc_get_dma_value(ADC_TypeDef* adc, std::size_t index);
uint32_t adc_get_rank_channel(ADC_TypeDef* adc, std::size_t rank_index);
uint32_t adc_get_rank_count(ADC_TypeDef* adc);
uint64_t adc_get_sequence_period_ns(ADC_TypeDef* adc);
uint64_t adc_get_pending_dma_completion_time_ns(ADC_TypeDef* adc);
uint64_t adc_get_completed_sequence_count(ADC_TypeDef* adc);
uint64_t adc_get_overrun_count(ADC_TypeDef* adc);

} // namespace ST_LIB::MockedHAL
