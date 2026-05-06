#include <array>
#include <tuple>

#include <gtest/gtest.h>

#include "HALAL/Models/DMA/DMA2.hpp"
#include "MockedDrivers/NVIC.hpp"
#include "MockedDrivers/mocked_hal_dma.hpp"

namespace ST_LIB::TestPanicReporter {
void reset();
void set_fail_on_error(bool enabled);
extern int call_count;
} // namespace ST_LIB::TestPanicReporter

extern "C" {
void DMA1_Stream0_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
}

namespace {

template <std::size_t N> struct DMAOnlyCtx {
    std::array<ST_LIB::DMADomain::Entry, N> entries{};
    std::size_t size = 0;

    template <typename D, typename Owner>
    consteval std::size_t add(typename D::Entry e, const Owner*) {
        const auto idx = size;
        entries[size++] = e;
        return idx;
    }
};

struct DMARequestInscribeResult {
    std::array<ST_LIB::DMADomain::Entry, 3> entries{};
    std::array<std::size_t, 3> indices{};
};

consteval DMARequestInscribeResult make_dma_request_inscribe_result() {
    DMAOnlyCtx<3> ctx{};
    ST_LIB::DMADomain::DMA<
        ST_LIB::DMADomain::Stream::dma1_stream6,
        ST_LIB::DMADomain::Stream::dma2_stream3,
        ST_LIB::DMADomain::Stream::none>
        dma_request(ST_LIB::DMADomain::Peripheral::spi4);

    const auto indices = dma_request.inscribe(ctx);
    return {.entries = ctx.entries, .indices = indices};
}

constexpr auto request_inscribe_result = make_dma_request_inscribe_result();
static_assert(request_inscribe_result.indices[0] == 0);
static_assert(request_inscribe_result.indices[1] == 1);
static_assert(request_inscribe_result.indices[2] == 2);
static_assert(request_inscribe_result.entries[0].instance == ST_LIB::DMADomain::Peripheral::spi4);
static_assert(request_inscribe_result.entries[0].stream == ST_LIB::DMADomain::Stream::dma1_stream6);
static_assert(request_inscribe_result.entries[0].irqn == DMA1_Stream6_IRQn);
static_assert(request_inscribe_result.entries[0].id == 0);
static_assert(request_inscribe_result.entries[1].stream == ST_LIB::DMADomain::Stream::dma2_stream3);
static_assert(request_inscribe_result.entries[1].irqn == DMA2_Stream3_IRQn);
static_assert(request_inscribe_result.entries[1].id == 1);
static_assert(request_inscribe_result.entries[2].stream == ST_LIB::DMADomain::Stream::none);
static_assert(request_inscribe_result.entries[2].irqn == static_cast<IRQn_Type>(0));
static_assert(request_inscribe_result.entries[2].id == 2);

constexpr std::array<ST_LIB::DMADomain::Entry, 2> spi_dma_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::spi2,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream0,
     .irqn = DMA1_Stream0_IRQn,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::spi2,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream1,
     .irqn = DMA1_Stream1_IRQn,
     .id = 1},
}};

constexpr auto spi_dma_cfg =
    ST_LIB::DMADomain::build<2>(std::span<const ST_LIB::DMADomain::Entry, 2>{spi_dma_entries});

static_assert(std::get<1>(spi_dma_cfg[0].init_data).Request == DMA_REQUEST_SPI2_RX);
static_assert(std::get<1>(spi_dma_cfg[1].init_data).Request == DMA_REQUEST_SPI2_TX);
static_assert(std::get<1>(spi_dma_cfg[0].init_data).Direction == DMA_PERIPH_TO_MEMORY);
static_assert(std::get<1>(spi_dma_cfg[1].init_data).Direction == DMA_MEMORY_TO_PERIPH);
static_assert(std::get<1>(spi_dma_cfg[0].init_data).FIFOThreshold == DMA_FIFO_THRESHOLD_FULL);
static_assert(std::get<1>(spi_dma_cfg[1].init_data).FIFOThreshold == DMA_FIFO_THRESHOLD_FULL);
static_assert(std::get<3>(spi_dma_cfg[0].init_data) == DMA1_Stream0_IRQn);
static_assert(std::get<3>(spi_dma_cfg[1].init_data) == DMA1_Stream1_IRQn);
static_assert(std::get<5>(spi_dma_cfg[0].init_data));
static_assert(std::get<5>(spi_dma_cfg[1].init_data));

constexpr std::array<ST_LIB::DMADomain::Entry, 2> auto_stream_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::adc1,
     .stream = ST_LIB::DMADomain::Stream::none,
     .irqn = 0,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::adc2,
     .stream = ST_LIB::DMADomain::Stream::none,
     .irqn = 0,
     .id = 0},
}};

constexpr auto auto_stream_cfg =
    ST_LIB::DMADomain::build<2>(std::span<const ST_LIB::DMADomain::Entry, 2>{auto_stream_entries});
static_assert(std::get<2>(auto_stream_cfg[0].init_data) == ST_LIB::DMADomain::Stream::dma1_stream0);
static_assert(std::get<2>(auto_stream_cfg[1].init_data) == ST_LIB::DMADomain::Stream::dma1_stream1);
static_assert(std::get<3>(auto_stream_cfg[0].init_data) == DMA1_Stream0_IRQn);
static_assert(std::get<3>(auto_stream_cfg[1].init_data) == DMA1_Stream1_IRQn);

constexpr std::array<ST_LIB::DMADomain::Entry, 3> mixed_stream_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::spi3,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream4,
     .irqn = DMA1_Stream4_IRQn,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::adc1,
     .stream = ST_LIB::DMADomain::Stream::none,
     .irqn = 0,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::adc2,
     .stream = ST_LIB::DMADomain::Stream::none,
     .irqn = 0,
     .id = 0},
}};

constexpr auto mixed_stream_cfg =
    ST_LIB::DMADomain::build<3>(std::span<const ST_LIB::DMADomain::Entry, 3>{mixed_stream_entries});
static_assert(
    std::get<2>(mixed_stream_cfg[0].init_data) == ST_LIB::DMADomain::Stream::dma1_stream4
);
static_assert(
    std::get<2>(mixed_stream_cfg[1].init_data) == ST_LIB::DMADomain::Stream::dma1_stream0
);
static_assert(
    std::get<2>(mixed_stream_cfg[2].init_data) == ST_LIB::DMADomain::Stream::dma1_stream1
);

constexpr std::array<ST_LIB::DMADomain::Entry, 1> adc_entry{{
    {.instance = ST_LIB::DMADomain::Peripheral::adc3,
     .stream = ST_LIB::DMADomain::Stream::dma2_stream4,
     .irqn = DMA2_Stream4_IRQn,
     .id = 0},
}};

constexpr auto adc_cfg =
    ST_LIB::DMADomain::build<1>(std::span<const ST_LIB::DMADomain::Entry, 1>{adc_entry});
static_assert(std::get<1>(adc_cfg[0].init_data).Request == DMA_REQUEST_ADC3);
static_assert(std::get<1>(adc_cfg[0].init_data).Direction == DMA_PERIPH_TO_MEMORY);
static_assert(std::get<1>(adc_cfg[0].init_data).PeriphInc == DMA_PINC_DISABLE);
static_assert(std::get<1>(adc_cfg[0].init_data).MemInc == DMA_MINC_ENABLE);
static_assert(std::get<1>(adc_cfg[0].init_data).PeriphDataAlignment == DMA_PDATAALIGN_HALFWORD);
static_assert(std::get<1>(adc_cfg[0].init_data).MemDataAlignment == DMA_MDATAALIGN_HALFWORD);
static_assert(std::get<1>(adc_cfg[0].init_data).Mode == DMA_CIRCULAR);
static_assert(std::get<1>(adc_cfg[0].init_data).Priority == DMA_PRIORITY_LOW);
static_assert(std::get<1>(adc_cfg[0].init_data).FIFOMode == DMA_FIFOMODE_DISABLE);
static_assert(!std::get<5>(adc_cfg[0].init_data));

constexpr std::array<ST_LIB::DMADomain::Entry, 3> fmac_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::fmac,
     .stream = ST_LIB::DMADomain::Stream::dma2_stream0,
     .irqn = DMA2_Stream0_IRQn,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::fmac,
     .stream = ST_LIB::DMADomain::Stream::dma2_stream1,
     .irqn = DMA2_Stream1_IRQn,
     .id = 1},
    {.instance = ST_LIB::DMADomain::Peripheral::fmac,
     .stream = ST_LIB::DMADomain::Stream::dma2_stream2,
     .irqn = DMA2_Stream2_IRQn,
     .id = 2},
}};

constexpr auto fmac_cfg =
    ST_LIB::DMADomain::build<3>(std::span<const ST_LIB::DMADomain::Entry, 3>{fmac_entries});
static_assert(std::get<1>(fmac_cfg[0].init_data).Direction == DMA_MEMORY_TO_MEMORY);
static_assert(std::get<1>(fmac_cfg[1].init_data).Direction == DMA_MEMORY_TO_PERIPH);
static_assert(std::get<1>(fmac_cfg[2].init_data).Direction == DMA_PERIPH_TO_MEMORY);
static_assert(std::get<1>(fmac_cfg[0].init_data).Request == DMA_REQUEST_MEM2MEM);
static_assert(std::get<1>(fmac_cfg[1].init_data).Request == DMA_REQUEST_FMAC_WRITE);
static_assert(std::get<1>(fmac_cfg[2].init_data).Request == DMA_REQUEST_FMAC_READ);

constexpr std::array<ST_LIB::DMADomain::Entry, 2> irq_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::spi2,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream0,
     .irqn = DMA1_Stream0_IRQn,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::spi2,
     .stream = ST_LIB::DMADomain::Stream::dma2_stream7,
     .irqn = DMA2_Stream7_IRQn,
     .id = 1},
}};

constexpr auto irq_cfg =
    ST_LIB::DMADomain::build<2>(std::span<const ST_LIB::DMADomain::Entry, 2>{irq_entries});

constexpr std::array<ST_LIB::DMADomain::Entry, 2> i2c_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::i2c2,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream2,
     .irqn = DMA1_Stream2_IRQn,
     .id = 0},
    {.instance = ST_LIB::DMADomain::Peripheral::i2c2,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream3,
     .irqn = DMA1_Stream3_IRQn,
     .id = 1},
}};

constexpr auto i2c_cfg =
    ST_LIB::DMADomain::build<2>(std::span<const ST_LIB::DMADomain::Entry, 2>{i2c_entries});

constexpr std::array<ST_LIB::DMADomain::Entry, 1> none_entries{{
    {.instance = ST_LIB::DMADomain::Peripheral::none,
     .stream = ST_LIB::DMADomain::Stream::dma1_stream4,
     .irqn = DMA1_Stream4_IRQn,
     .id = 0},
}};

constexpr auto none_cfg =
    ST_LIB::DMADomain::build<1>(std::span<const ST_LIB::DMADomain::Entry, 1>{none_entries});

void clear_nvic_enables() {
    for (auto& reg : NVIC->ISER) {
        reg = 0U;
    }
}

void clear_dma_irq_table() {
    for (auto& slot : dma_irq_table) {
        slot = nullptr;
    }
}

} // namespace

class DMA2Test : public ::testing::Test {
protected:
    void SetUp() override {
        ST_LIB::MockedHAL::dma_reset();
        ST_LIB::TestPanicReporter::reset();
        clear_nvic_enables();
        clear_dma_irq_table();
    }
};

TEST_F(DMA2Test, InitConfiguresStreamsNVICAndLookupTable) {
    ST_LIB::DMADomain::Init<2>::init(spi_dma_cfg);

    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::Init), 2U);

    auto& dma0 = ST_LIB::DMADomain::Init<2>::instances[0].dma;
    auto& dma1 = ST_LIB::DMADomain::Init<2>::instances[1].dma;
    EXPECT_EQ(dma0.Instance, DMA1_Stream0);
    EXPECT_EQ(dma1.Instance, DMA1_Stream1);

    EXPECT_EQ(dma0.Init.Request, DMA_REQUEST_SPI2_RX);
    EXPECT_EQ(dma1.Init.Request, DMA_REQUEST_SPI2_TX);
    EXPECT_EQ(dma0.Init.Direction, DMA_PERIPH_TO_MEMORY);
    EXPECT_EQ(dma1.Init.Direction, DMA_MEMORY_TO_PERIPH);

    EXPECT_EQ(dma_irq_table[0], &dma0);
    EXPECT_EQ(dma_irq_table[1], &dma1);
}

TEST_F(DMA2Test, InitKeepsADCDMARegisteredButLeavesItsNVICDisabled) {
    ST_LIB::DMADomain::Init<1>::init(adc_cfg);

    auto& dma = ST_LIB::DMADomain::Init<1>::instances[0].dma;

    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::Init), 1U);
    EXPECT_EQ(dma.Instance, DMA2_Stream4);
    EXPECT_EQ(dma_irq_table[12], &dma);
    EXPECT_EQ(NVIC_GetEnableIRQ(DMA2_Stream4_IRQn), 0U);
}

TEST_F(DMA2Test, StartForwardsTransferParametersToHALDMA) {
    ST_LIB::DMADomain::Init<2>::init(spi_dma_cfg);
    auto& instance = ST_LIB::DMADomain::Init<2>::instances[0];

    instance.start(0x1111U, 0x2222U, 128U);

    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::StartIT), 1U);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_start_handle(), &instance.dma);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_start_src(), 0x1111U);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_start_dst(), 0x2222U);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_start_length(), 128U);
}

TEST_F(DMA2Test, ScheduledTransferTimingSerializesSharedBusUsage) {
    ST_LIB::DMADomain::Init<2>::init(spi_dma_cfg);
    auto& rx = ST_LIB::DMADomain::Init<2>::instances[0].dma;
    auto& tx = ST_LIB::DMADomain::Init<2>::instances[1].dma;
    ST_LIB::MockedHAL::dma_set_transfer_timing(100ULL, 10ULL);

    const uint64_t first_completion = ST_LIB::MockedHAL::dma_schedule_transfer(&rx, 1'000ULL, 4U);
    const uint64_t second_completion = ST_LIB::MockedHAL::dma_schedule_transfer(&tx, 1'000ULL, 4U);

    EXPECT_EQ(first_completion, 1'140ULL);
    EXPECT_EQ(second_completion, 1'280ULL);
    EXPECT_EQ(
        ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::ScheduleTransfer),
        2U
    );
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_scheduled_handle(), &tx);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_schedule_request_time_ns(), 1'000ULL);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_schedule_completion_time_ns(), 1'280ULL);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_schedule_bytes(), 4U);
}

TEST_F(DMA2Test, InitFailureTriggersErrorAndSkipsRegistration) {
    ST_LIB::TestPanicReporter::set_fail_on_error(false);
    ST_LIB::MockedHAL::dma_set_init_status(HAL_ERROR);

    ST_LIB::DMADomain::Init<2>::init(spi_dma_cfg);

    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::Init), 2U);
    EXPECT_EQ(ST_LIB::TestPanicReporter::call_count, 2);
    EXPECT_EQ(dma_irq_table[0], nullptr);
    EXPECT_EQ(dma_irq_table[1], nullptr);
}

TEST_F(DMA2Test, InitAppliesI2CDirectionAndAlignmentAtRuntime) {
    ST_LIB::DMADomain::Init<2>::init(i2c_cfg);

    auto& rx = ST_LIB::DMADomain::Init<2>::instances[0].dma;
    auto& tx = ST_LIB::DMADomain::Init<2>::instances[1].dma;

    EXPECT_EQ(rx.Init.Request, DMA_REQUEST_I2C2_RX);
    EXPECT_EQ(tx.Init.Request, DMA_REQUEST_I2C2_TX);
    EXPECT_EQ(rx.Init.Direction, DMA_PERIPH_TO_MEMORY);
    EXPECT_EQ(tx.Init.Direction, DMA_MEMORY_TO_PERIPH);
    EXPECT_EQ(rx.Init.MemDataAlignment, DMA_MDATAALIGN_WORD);
    EXPECT_EQ(tx.Init.MemDataAlignment, DMA_MDATAALIGN_WORD);
    EXPECT_EQ(rx.Init.Mode, DMA_CIRCULAR);
    EXPECT_EQ(tx.Init.Mode, DMA_CIRCULAR);
}

TEST_F(DMA2Test, InitUsesPrecomputedDMAInitDataWithoutRuntimeReconfiguration) {
    auto mutated_cfg = spi_dma_cfg;
    std::get<0>(mutated_cfg[0].init_data) = ST_LIB::DMADomain::Peripheral::adc1;
    std::get<4>(mutated_cfg[0].init_data) = 0;
    std::get<0>(mutated_cfg[1].init_data) = ST_LIB::DMADomain::Peripheral::adc2;
    std::get<4>(mutated_cfg[1].init_data) = 0;
    std::get<5>(mutated_cfg[0].init_data) = true;
    std::get<5>(mutated_cfg[1].init_data) = true;

    ST_LIB::DMADomain::Init<2>::init(mutated_cfg);

    auto& rx = ST_LIB::DMADomain::Init<2>::instances[0].dma;
    auto& tx = ST_LIB::DMADomain::Init<2>::instances[1].dma;

    EXPECT_EQ(rx.Init.Request, DMA_REQUEST_SPI2_RX);
    EXPECT_EQ(tx.Init.Request, DMA_REQUEST_SPI2_TX);
    EXPECT_EQ(rx.Init.Direction, DMA_PERIPH_TO_MEMORY);
    EXPECT_EQ(tx.Init.Direction, DMA_MEMORY_TO_PERIPH);
}

TEST_F(DMA2Test, InitRejectsCorruptedConfigWithNoAssignedStream) {
    ST_LIB::TestPanicReporter::set_fail_on_error(false);

    auto corrupted_cfg = spi_dma_cfg;
    std::get<2>(corrupted_cfg[0].init_data) = ST_LIB::DMADomain::Stream::none;
    std::get<3>(corrupted_cfg[0].init_data) = static_cast<IRQn_Type>(0);

    ST_LIB::DMADomain::Init<2>::init(corrupted_cfg);

    EXPECT_EQ(ST_LIB::TestPanicReporter::call_count, 1);
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::Init), 1U);
    EXPECT_EQ(dma_irq_table[0], nullptr);
    EXPECT_EQ(dma_irq_table[1], &ST_LIB::DMADomain::Init<2>::instances[1].dma);
}

TEST_F(DMA2Test, InitAppliesNonePeripheralSettingsAtRuntime) {
    ST_LIB::DMADomain::Init<1>::init(none_cfg);

    auto& inst = ST_LIB::DMADomain::Init<1>::instances[0].dma;
    EXPECT_EQ(inst.Init.Request, DMA_REQUEST_MEM2MEM);
    EXPECT_EQ(inst.Init.Direction, DMA_MEMORY_TO_MEMORY);
    EXPECT_EQ(inst.Init.PeriphInc, DMA_PINC_ENABLE);
    EXPECT_EQ(inst.Init.PeriphDataAlignment, DMA_PDATAALIGN_WORD);
    EXPECT_EQ(inst.Init.Mode, DMA_NORMAL);
}

TEST_F(DMA2Test, InitAppliesFMACSpecialSettingsAtRuntime) {
    ST_LIB::DMADomain::Init<3>::init(fmac_cfg);

    auto& m2m = ST_LIB::DMADomain::Init<3>::instances[0].dma;
    auto& write = ST_LIB::DMADomain::Init<3>::instances[1].dma;
    auto& read = ST_LIB::DMADomain::Init<3>::instances[2].dma;

    EXPECT_EQ(m2m.Init.Request, DMA_REQUEST_MEM2MEM);
    EXPECT_EQ(write.Init.Request, DMA_REQUEST_FMAC_WRITE);
    EXPECT_EQ(read.Init.Request, DMA_REQUEST_FMAC_READ);

    EXPECT_EQ(m2m.Init.Direction, DMA_MEMORY_TO_MEMORY);
    EXPECT_EQ(write.Init.Direction, DMA_MEMORY_TO_PERIPH);
    EXPECT_EQ(read.Init.Direction, DMA_PERIPH_TO_MEMORY);

    EXPECT_EQ(m2m.Init.FIFOMode, DMA_FIFOMODE_ENABLE);
    EXPECT_EQ(write.Init.FIFOMode, DMA_FIFOMODE_ENABLE);
    EXPECT_EQ(read.Init.FIFOMode, DMA_FIFOMODE_ENABLE);

    EXPECT_EQ(m2m.Init.Priority, DMA_PRIORITY_HIGH);
    EXPECT_EQ(write.Init.Priority, DMA_PRIORITY_HIGH);
    EXPECT_EQ(read.Init.Priority, DMA_PRIORITY_HIGH);
}

TEST_F(DMA2Test, IRQHandlersDispatchMappedDMAHandles) {
    ST_LIB::DMADomain::Init<2>::init(irq_cfg);
    auto& dma0 = ST_LIB::DMADomain::Init<2>::instances[0].dma;
    auto& dma1 = ST_LIB::DMADomain::Init<2>::instances[1].dma;

    DMA1_Stream0_IRQHandler();
    EXPECT_EQ(
        ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::IRQHandler),
        1U
    );
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_irq_handle(), &dma0);

    DMA2_Stream7_IRQHandler();
    EXPECT_EQ(
        ST_LIB::MockedHAL::dma_get_call_count(ST_LIB::MockedHAL::DMAOperation::IRQHandler),
        2U
    );
    EXPECT_EQ(ST_LIB::MockedHAL::dma_get_last_irq_handle(), &dma1);
}
