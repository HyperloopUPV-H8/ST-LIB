#include "MockedDrivers/mocked_hal_adc.hpp"
#include "MockedDrivers/mocked_hal_dma.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <unordered_map>
#include <utility>

ADC_HandleTypeDef hadc1{};
ADC_HandleTypeDef hadc2{};
ADC_HandleTypeDef hadc3{};

namespace {

constexpr std::size_t kMaxAdcRanks = 16;
constexpr std::size_t kAdcOperationCount =
    static_cast<std::size_t>(ST_LIB::MockedHAL::ADCOperation::StopDMA) + 1U;

struct ADCPeripheralState {
    struct PendingDMAWrite {
        bool active = false;
        uint64_t completion_time_ns = 0;
        uint32_t length = 0;
        std::array<uint16_t, kMaxAdcRanks> samples{};
    };

    bool initialized = false;
    bool configured = false;
    bool running = false;
    bool dma_running = false;
    bool force_poll_timeout = false;
    bool timed_dma_enabled = false;
    uint32_t active_channel = ADC_CHANNEL_0;
    uint32_t last_raw = 0;
    void* dma_buffer = nullptr;
    uint32_t dma_length = 0;
    uint32_t dma_mem_alignment = DMA_MDATAALIGN_HALFWORD;
    uint64_t kernel_clock_hz = 64'000'000ULL;
    uint64_t sequence_period_ns = 0;
    uint64_t next_sequence_time_ns = 0;
    uint64_t completed_sequence_count = 0;
    uint64_t overrun_count = 0;
    uint8_t rank_count = 0;
    std::array<uint32_t, kMaxAdcRanks> rank_channels{};
    std::array<uint32_t, kMaxAdcRanks> rank_sample_times{};
    std::unordered_map<uint32_t, uint32_t> channel_raw_values{};
    std::unordered_map<uint32_t, ST_LIB::MockedHAL::ADCSignalGenerator> channel_generators{};
    PendingDMAWrite pending_dma{};
};

struct ADCMockState {
    std::array<std::size_t, kAdcOperationCount> calls{};
};

#if STLIB_HAS_ADC3
constexpr std::size_t kAdcCount = 3;
static std::array<ADC_TypeDef*, kAdcCount> adc_instances{ADC1, ADC2, ADC3};
#else
constexpr std::size_t kAdcCount = 2;
static std::array<ADC_TypeDef*, kAdcCount> adc_instances{ADC1, ADC2};
#endif

static std::array<ADCPeripheralState, kAdcCount> adc_states{};
static ADCMockState g_mock_state{};
static uint64_t g_sim_time_ns = 0;

static ADC_HandleTypeDef* handle_for(ADC_TypeDef* instance) {
    if (instance == ADC1) {
        return &hadc1;
    }
    if (instance == ADC2) {
        return &hadc2;
    }
    if (instance == ADC3) {
        return &hadc3;
    }
    return nullptr;
}

static ADCPeripheralState* try_state_for(ADC_TypeDef* instance) {
    for (std::size_t i = 0; i < kAdcCount; ++i) {
        if (adc_instances[i] == instance) {
            return &adc_states[i];
        }
    }
    return nullptr;
}

static uint32_t resolution_mask(uint32_t resolution) {
    switch (resolution) {
    case ADC_RESOLUTION_16B:
        return 0xFFFFU;
    case ADC_RESOLUTION_14B:
        return 0x3FFFU;
    case ADC_RESOLUTION_12B:
        return 0x0FFFU;
    case ADC_RESOLUTION_10B:
        return 0x03FFU;
    case ADC_RESOLUTION_8B:
        return 0x00FFU;
    default:
        return 0x0FFFU;
    }
}

static uint8_t decode_regular_rank(uint32_t rank) {
    switch (rank) {
    case ADC_REGULAR_RANK_1:
        return 1U;
    case ADC_REGULAR_RANK_2:
        return 2U;
    case ADC_REGULAR_RANK_3:
        return 3U;
    case ADC_REGULAR_RANK_4:
        return 4U;
    case ADC_REGULAR_RANK_5:
        return 5U;
    case ADC_REGULAR_RANK_6:
        return 6U;
    case ADC_REGULAR_RANK_7:
        return 7U;
    case ADC_REGULAR_RANK_8:
        return 8U;
    case ADC_REGULAR_RANK_9:
        return 9U;
    case ADC_REGULAR_RANK_10:
        return 10U;
    case ADC_REGULAR_RANK_11:
        return 11U;
    case ADC_REGULAR_RANK_12:
        return 12U;
    case ADC_REGULAR_RANK_13:
        return 13U;
    case ADC_REGULAR_RANK_14:
        return 14U;
    case ADC_REGULAR_RANK_15:
        return 15U;
    case ADC_REGULAR_RANK_16:
        return 16U;
    default:
        return 0U;
    }
}

static uint32_t sample_time_cycles_x2(uint32_t sample_time) {
    switch (sample_time) {
    case ADC_SAMPLETIME_1CYCLE_5:
        return 3U;
    case ADC_SAMPLETIME_2CYCLES_5:
        return 5U;
    case ADC_SAMPLETIME_8CYCLES_5:
        return 17U;
    case ADC_SAMPLETIME_16CYCLES_5:
        return 33U;
    case ADC_SAMPLETIME_32CYCLES_5:
        return 65U;
    case ADC_SAMPLETIME_64CYCLES_5:
        return 129U;
    case ADC_SAMPLETIME_387CYCLES_5:
        return 775U;
    case ADC_SAMPLETIME_810CYCLES_5:
        return 1621U;
    default:
        return 17U;
    }
}

static uint32_t conversion_cycles_x2(uint32_t resolution) {
    switch (resolution) {
    case ADC_RESOLUTION_8B:
        return 17U;
    case ADC_RESOLUTION_10B:
        return 21U;
    case ADC_RESOLUTION_12B:
        return 25U;
    case ADC_RESOLUTION_14B:
        return 29U;
    case ADC_RESOLUTION_16B:
        return 33U;
    default:
        return 25U;
    }
}

static uint32_t prescaler_divisor(uint32_t prescaler) {
    switch (prescaler) {
    case ADC_CLOCK_ASYNC_DIV1:
        return 1U;
    case ADC_CLOCK_ASYNC_DIV2:
        return 2U;
    case ADC_CLOCK_ASYNC_DIV4:
        return 4U;
    case ADC_CLOCK_ASYNC_DIV6:
        return 6U;
    case ADC_CLOCK_ASYNC_DIV8:
        return 8U;
    case ADC_CLOCK_ASYNC_DIV10:
        return 10U;
    case ADC_CLOCK_ASYNC_DIV12:
        return 12U;
    case ADC_CLOCK_ASYNC_DIV16:
        return 16U;
    case ADC_CLOCK_ASYNC_DIV32:
        return 32U;
    case ADC_CLOCK_ASYNC_DIV64:
        return 64U;
    case ADC_CLOCK_ASYNC_DIV128:
        return 128U;
    case ADC_CLOCK_ASYNC_DIV256:
        return 256U;
    default:
        return 1U;
    }
}

static uint32_t alignment_bytes(uint32_t alignment) {
    switch (alignment) {
    case DMA_MDATAALIGN_BYTE:
        return 1U;
    case DMA_MDATAALIGN_WORD:
        return 4U;
    case DMA_MDATAALIGN_HALFWORD:
    default:
        return 2U;
    }
}

static uint64_t ceil_div_u64(uint64_t numerator, uint64_t denominator) {
    return (numerator + denominator - 1ULL) / denominator;
}

static void write_dma_sample(ADCPeripheralState& state, std::size_t index, uint32_t raw) {
    if (state.dma_buffer == nullptr) {
        return;
    }

    switch (state.dma_mem_alignment) {
    case DMA_MDATAALIGN_BYTE:
        reinterpret_cast<uint8_t*>(state.dma_buffer)[index] = static_cast<uint8_t>(raw);
        break;
    case DMA_MDATAALIGN_WORD:
        reinterpret_cast<uint32_t*>(state.dma_buffer)[index] = raw;
        break;
    case DMA_MDATAALIGN_HALFWORD:
    default:
        reinterpret_cast<uint16_t*>(state.dma_buffer)[index] = static_cast<uint16_t>(raw);
        break;
    }
}

static uint32_t read_dma_sample(const ADCPeripheralState& state, std::size_t index) {
    if (state.dma_buffer == nullptr) {
        return 0U;
    }

    switch (state.dma_mem_alignment) {
    case DMA_MDATAALIGN_BYTE:
        return reinterpret_cast<const uint8_t*>(state.dma_buffer)[index];
    case DMA_MDATAALIGN_WORD:
        return reinterpret_cast<const uint32_t*>(state.dma_buffer)[index];
    case DMA_MDATAALIGN_HALFWORD:
    default:
        return reinterpret_cast<const uint16_t*>(state.dma_buffer)[index];
    }
}

static uint32_t sample_channel_raw(
    const ADCPeripheralState& state,
    ADC_HandleTypeDef* hadc,
    uint32_t channel,
    uint64_t time_ns
) {
    const auto generator_it = state.channel_generators.find(channel);
    const uint32_t raw =
        (generator_it == state.channel_generators.end()) ? [&]() {
            const auto raw_it = state.channel_raw_values.find(channel);
            return (raw_it == state.channel_raw_values.end()) ? 0U : raw_it->second;
        }()
                                                         : generator_it->second(time_ns);
    return raw & resolution_mask(hadc->Init.Resolution);
}

static void refresh_last_raw_from_active_channel(
    ADCPeripheralState& state,
    ADC_HandleTypeDef* hadc,
    uint64_t time_ns
) {
    state.last_raw = sample_channel_raw(state, hadc, state.active_channel, time_ns);
}

static uint64_t
compute_sequence_period_ns(const ADCPeripheralState& state, ADC_HandleTypeDef* hadc) {
    if (hadc == nullptr || state.rank_count == 0U || state.kernel_clock_hz == 0U) {
        return 0U;
    }

    uint64_t total_cycles_x2 = 0;
    for (uint8_t i = 0; i < state.rank_count; ++i) {
        const uint32_t sample_cycles = sample_time_cycles_x2(state.rank_sample_times[i]);
        total_cycles_x2 += sample_cycles + conversion_cycles_x2(hadc->Init.Resolution);
    }

    const uint64_t scaled_cycles_x2 =
        total_cycles_x2 * static_cast<uint64_t>(prescaler_divisor(hadc->Init.ClockPrescaler));
    return ceil_div_u64(scaled_cycles_x2 * 1'000'000'000ULL, 2ULL * state.kernel_clock_hz);
}

static void apply_pending_dma_if_due(ADCPeripheralState& state, uint64_t up_to_time_ns) {
    if (!state.pending_dma.active || state.pending_dma.completion_time_ns > up_to_time_ns) {
        return;
    }

    for (std::size_t i = 0; i < state.pending_dma.length; ++i) {
        write_dma_sample(state, i, state.pending_dma.samples[i]);
    }
    if (state.pending_dma.length != 0U) {
        state.last_raw = state.pending_dma.samples[0];
    }
    state.pending_dma = {};
    state.completed_sequence_count++;
}

static void process_timed_sequence(
    ADCPeripheralState& state,
    ADC_HandleTypeDef* hadc,
    uint64_t sequence_time_ns
) {
    if (!state.dma_running || state.dma_buffer == nullptr || state.rank_count == 0U) {
        return;
    }

    apply_pending_dma_if_due(state, sequence_time_ns);

    if (state.pending_dma.active) {
        state.overrun_count++;
        return;
    }

    const uint32_t length = std::min<uint32_t>(state.rank_count, state.dma_length);
    std::array<uint16_t, kMaxAdcRanks> samples{};
    for (uint32_t i = 0; i < length; ++i) {
        samples[i] = static_cast<uint16_t>(
            sample_channel_raw(state, hadc, state.rank_channels[i], sequence_time_ns)
        );
    }

    const uint32_t bytes = length * alignment_bytes(state.dma_mem_alignment);
    const uint64_t completion_time =
        ST_LIB::MockedHAL::dma_schedule_transfer(hadc->DMA_Handle, sequence_time_ns, bytes);

    if (completion_time <= sequence_time_ns) {
        for (uint32_t i = 0; i < length; ++i) {
            write_dma_sample(state, i, samples[i]);
        }
        if (length != 0U) {
            state.last_raw = samples[0];
        }
        state.completed_sequence_count++;
        return;
    }

    state.pending_dma.active = true;
    state.pending_dma.completion_time_ns = completion_time;
    state.pending_dma.length = length;
    state.pending_dma.samples = samples;
}

static void refresh_dma_buffer(ADCPeripheralState& state, ADC_HandleTypeDef* hadc) {
    if (state.dma_buffer == nullptr || state.dma_length == 0) {
        refresh_last_raw_from_active_channel(state, hadc, g_sim_time_ns);
        return;
    }

    const auto mask = resolution_mask(hadc->Init.Resolution);
    const auto length = std::min<std::size_t>(state.dma_length, kMaxAdcRanks);
    for (std::size_t i = 0; i < length; ++i) {
        const auto channel = state.rank_channels[i];
        write_dma_sample(state, i, sample_channel_raw(state, hadc, channel, g_sim_time_ns) & mask);
    }
    refresh_last_raw_from_active_channel(state, hadc, g_sim_time_ns);
}

static void increment_call(ST_LIB::MockedHAL::ADCOperation op) {
    g_mock_state.calls[static_cast<std::size_t>(op)]++;
}

} // namespace

namespace ST_LIB::MockedHAL {

void adc_reset() {
    for (auto& state : adc_states) {
        state = {};
    }
    hadc1 = {};
    hadc2 = {};
    hadc3 = {};
    g_mock_state = {};
    g_sim_time_ns = 0;
}

void adc_set_channel_raw(ADC_TypeDef* adc, uint32_t channel, uint32_t raw_value) {
    auto* state = try_state_for(adc);
    if (state == nullptr) {
        return;
    }
    state->channel_raw_values[channel] = raw_value;

    ADC_HandleTypeDef* hadc = handle_for(adc);

    if (hadc != nullptr && state->dma_running && !state->timed_dma_enabled) {
        refresh_dma_buffer(*state, hadc);
    }
}

void adc_set_channel_generator(ADC_TypeDef* adc, uint32_t channel, ADCSignalGenerator generator) {
    auto* state = try_state_for(adc);
    if (state == nullptr) {
        return;
    }
    state->channel_generators[channel] = std::move(generator);
}

void adc_set_poll_timeout(ADC_TypeDef* adc, bool enabled) {
    auto* state = try_state_for(adc);
    if (state == nullptr) {
        return;
    }
    state->force_poll_timeout = enabled;
}

void adc_enable_timed_dma(ADC_TypeDef* adc, bool enabled) {
    auto* state = try_state_for(adc);
    if (state == nullptr) {
        return;
    }
    state->timed_dma_enabled = enabled;
}

void adc_set_kernel_clock_hz(ADC_TypeDef* adc, uint64_t kernel_clock_hz) {
    auto* state = try_state_for(adc);
    if (state == nullptr) {
        return;
    }
    state->kernel_clock_hz = kernel_clock_hz;
}

void adc_advance_time_ns(uint64_t delta_ns) {
    const uint64_t end_time_ns = g_sim_time_ns + delta_ns;

    for (std::size_t i = 0; i < kAdcCount; ++i) {
        auto* adc = adc_instances[i];
        auto& state = adc_states[i];
        if (adc == nullptr || !state.timed_dma_enabled || !state.dma_running ||
            state.sequence_period_ns == 0U) {
            continue;
        }

        ADC_HandleTypeDef* hadc = handle_for(adc);
        if (hadc == nullptr) {
            continue;
        }

        while (true) {
            const uint64_t next_dma_completion = state.pending_dma.active
                                                     ? state.pending_dma.completion_time_ns
                                                     : std::numeric_limits<uint64_t>::max();
            const uint64_t next_sequence = state.next_sequence_time_ns == 0U
                                               ? std::numeric_limits<uint64_t>::max()
                                               : state.next_sequence_time_ns;
            const uint64_t next_event = std::min(next_dma_completion, next_sequence);

            if (next_event > end_time_ns) {
                break;
            }

            if (next_dma_completion <= next_sequence) {
                apply_pending_dma_if_due(state, next_dma_completion);
            } else {
                process_timed_sequence(state, hadc, next_sequence);
                state.next_sequence_time_ns += state.sequence_period_ns;
            }
        }

        apply_pending_dma_if_due(state, end_time_ns);
    }

    g_sim_time_ns = end_time_ns;
}

uint64_t adc_get_time_ns() { return g_sim_time_ns; }

uint32_t adc_get_last_channel(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->active_channel;
}

bool adc_is_running(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return state != nullptr && state->running;
}

bool adc_is_dma_running(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return state != nullptr && state->dma_running;
}

std::size_t adc_get_call_count(ADCOperation op) {
    return g_mock_state.calls[static_cast<std::size_t>(op)];
}

uint32_t adc_get_dma_length(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->dma_length;
}

uint32_t adc_get_dma_value(ADC_TypeDef* adc, std::size_t index) {
    const auto* state = try_state_for(adc);
    if (state == nullptr || state->dma_buffer == nullptr || index >= state->dma_length) {
        return 0U;
    }
    return read_dma_sample(*state, index);
}

uint32_t adc_get_rank_channel(ADC_TypeDef* adc, std::size_t rank_index) {
    const auto* state = try_state_for(adc);
    if (state == nullptr || rank_index >= state->rank_count) {
        return 0U;
    }
    return state->rank_channels[rank_index];
}

uint32_t adc_get_rank_count(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->rank_count;
}

uint64_t adc_get_sequence_period_ns(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->sequence_period_ns;
}

uint64_t adc_get_pending_dma_completion_time_ns(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    if (state == nullptr) {
        return 0U;
    }
    return state->pending_dma.active ? state->pending_dma.completion_time_ns : 0U;
}

uint64_t adc_get_completed_sequence_count(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->completed_sequence_count;
}

uint64_t adc_get_overrun_count(ADC_TypeDef* adc) {
    const auto* state = try_state_for(adc);
    return (state == nullptr) ? 0U : state->overrun_count;
}

} // namespace ST_LIB::MockedHAL

extern "C" HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::Init);

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr) {
        return HAL_ERROR;
    }
    const bool timed_dma_enabled = state->timed_dma_enabled;
    const uint64_t kernel_clock_hz = state->kernel_clock_hz;
    const auto channel_raw_values = state->channel_raw_values;
    const auto channel_generators = state->channel_generators;
    state->initialized = true;
    state->running = false;
    state->dma_running = false;
    state->configured = false;
    state->last_raw = 0;
    state->dma_buffer = nullptr;
    state->dma_length = 0;
    state->dma_mem_alignment = DMA_MDATAALIGN_HALFWORD;
    state->sequence_period_ns = 0;
    state->next_sequence_time_ns = 0;
    state->completed_sequence_count = 0;
    state->overrun_count = 0;
    state->rank_count = 0;
    state->rank_channels.fill(ADC_CHANNEL_0);
    state->rank_sample_times.fill(ADC_SAMPLETIME_1CYCLE_5);
    state->pending_dma = {};
    state->timed_dma_enabled = timed_dma_enabled;
    state->kernel_clock_hz = kernel_clock_hz;
    state->channel_raw_values = channel_raw_values;
    state->channel_generators = channel_generators;

    hadc->State = HAL_ADC_STATE_READY;
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef* hadc) {
    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr) {
        return HAL_ERROR;
    }
    *state = {};
    hadc->State = HAL_ADC_STATE_RESET;
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;
    hadc->DMA_Handle = nullptr;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef
HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::ConfigChannel);

    if (hadc == nullptr || hadc->Instance == nullptr || sConfig == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr || !state->initialized) {
        return HAL_ERROR;
    }

    const uint8_t decoded_rank = decode_regular_rank(sConfig->Rank);
    if (decoded_rank == 0U) {
        return HAL_ERROR;
    }

    state->active_channel = sConfig->Channel;
    state->rank_channels[decoded_rank - 1U] = sConfig->Channel;
    state->rank_sample_times[decoded_rank - 1U] = sConfig->SamplingTime;
    state->rank_count = std::max<uint8_t>(state->rank_count, decoded_rank);
    state->configured = true;
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::Start);

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr || !state->initialized || !state->configured) {
        return HAL_ERROR;
    }
    if (state->running) {
        return HAL_BUSY;
    }

    state->running = true;
    hadc->State |= HAL_ADC_STATE_REG_BUSY;
    hadc->State &= ~HAL_ADC_STATE_REG_EOC;
    hadc->State &= ~HAL_ADC_STATE_TIMEOUT;
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef
HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::StartDMA);

    if (hadc == nullptr || hadc->Instance == nullptr || pData == nullptr ||
        hadc->DMA_Handle == nullptr || Length == 0U) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr || !state->initialized || !state->configured) {
        return HAL_ERROR;
    }

    state->running = true;
    state->dma_running = true;
    state->dma_buffer = pData;
    state->dma_length = Length;
    state->dma_mem_alignment = hadc->DMA_Handle->Init.MemDataAlignment;
    state->pending_dma = {};
    state->completed_sequence_count = 0;
    state->overrun_count = 0;
    state->sequence_period_ns = compute_sequence_period_ns(*state, hadc);
    state->next_sequence_time_ns =
        state->timed_dma_enabled ? (g_sim_time_ns + state->sequence_period_ns) : 0U;

    const auto dma_status = HAL_DMA_Start_IT(
        hadc->DMA_Handle,
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&hadc->Instance->DR)),
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pData)),
        Length
    );
    if (dma_status != HAL_OK) {
        state->running = false;
        state->dma_running = false;
        state->dma_buffer = nullptr;
        state->dma_length = 0;
        return dma_status;
    }

    if (!state->timed_dma_enabled) {
        refresh_dma_buffer(*state, hadc);
    }

    hadc->State |= HAL_ADC_STATE_REG_BUSY;
    hadc->State &= ~HAL_ADC_STATE_TIMEOUT;
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::PollForConversion);
    (void)Timeout;

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr || !state->running) {
        return HAL_ERROR;
    }

    if (state->force_poll_timeout) {
        hadc->State |= HAL_ADC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
    }

    refresh_last_raw_from_active_channel(*state, hadc, g_sim_time_ns);

    hadc->State &= ~HAL_ADC_STATE_TIMEOUT;
    hadc->State |= HAL_ADC_STATE_REG_EOC;
    return HAL_OK;
}

extern "C" uint32_t HAL_ADC_GetValue(const ADC_HandleTypeDef* hadc) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::GetValue);

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return 0;
    }
    const auto* state = try_state_for(hadc->Instance);
    return (state == nullptr) ? 0U : state->last_raw;
}

extern "C" HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::Stop);

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr) {
        return HAL_ERROR;
    }
    state->running = false;
    state->pending_dma = {};
    state->next_sequence_time_ns = 0;

    hadc->State &= ~HAL_ADC_STATE_REG_BUSY;
    hadc->State &= ~HAL_ADC_STATE_REG_EOC;
    return HAL_OK;
}

extern "C" HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc) {
    increment_call(ST_LIB::MockedHAL::ADCOperation::StopDMA);

    if (hadc == nullptr || hadc->Instance == nullptr) {
        return HAL_ERROR;
    }

    auto* state = try_state_for(hadc->Instance);
    if (state == nullptr) {
        return HAL_ERROR;
    }
    state->running = false;
    state->dma_running = false;
    state->pending_dma = {};
    state->next_sequence_time_ns = 0;

    hadc->State &= ~HAL_ADC_STATE_REG_BUSY;
    hadc->State &= ~HAL_ADC_STATE_REG_EOC;
    return HAL_OK;
}

extern "C" uint32_t HAL_ADC_GetState(const ADC_HandleTypeDef* hadc) {
    if (hadc == nullptr) {
        return HAL_ADC_STATE_RESET;
    }
    return hadc->State;
}

extern "C" uint32_t HAL_ADC_GetError(const ADC_HandleTypeDef* hadc) {
    if (hadc == nullptr) {
        return HAL_ADC_ERROR_NONE;
    }
    return hadc->ErrorCode;
}

extern "C" void
HAL_SYSCFG_AnalogSwitchConfig(uint32_t SYSCFG_AnalogSwitch, uint32_t SYSCFG_SwitchState) {
    (void)SYSCFG_AnalogSwitch;
    (void)SYSCFG_SwitchState;
}
