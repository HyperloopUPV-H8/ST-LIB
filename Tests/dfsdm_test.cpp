#include <array>

#include <gtest/gtest.h>

#include "HALAL/Models/DMA/DMA2.hpp"
#include "HALAL/Models/GPIO.hpp"
#include "HALAL/Services/DFSDM/DFSDM.hpp"
#include "MockedDrivers/NVIC.hpp"
#include "MockedDrivers/mocked_hal_dma.hpp"

namespace ST_LIB::TestErrorHandler {
void reset();
void set_fail_on_error(bool enabled);
extern int call_count;
} // namespace ST_LIB::TestErrorHandler

namespace {

// DFSDM Channel configuration
constexpr ST_LIB::DFSDM_CHANNEL_DOMAIN::Config_Channel dfsdm_channel_cfg{
    .offset = 0,
    .right_shift = 0,
    .spi_clock_sel = ST_LIB::DFSDM_CHANNEL_DOMAIN::SPICKSel::CLK_DIVIDED_2_RISING,
    .spi_type = ST_LIB::DFSDM_CHANNEL_DOMAIN::SPI_Type::SPI_RISING,
    .clock_absence = ST_LIB::DFSDM_CHANNEL_DOMAIN::Clock_Absence::Disable,
    .short_circuit = ST_LIB::DFSDM_CHANNEL_DOMAIN::Short_Circuit::Disable,
    .extreme_detector = ST_LIB::DFSDM_CHANNEL_DOMAIN::Extreme_Detector::Disable,
};

// DFSDM Filter configuration
constexpr ST_LIB::DFSDM_CHANNEL_DOMAIN::Config_Filter dfsdm_filter_cfg{
    .filter = 0,
    .trigger_conv = ST_LIB::DFSDM_CHANNEL_DOMAIN::Trigger_Timer_Source::Unused,
    .filter_type = ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::FastSinc,
    .oversampling = 32,
    .integrator = 1,
    .type_conv = ST_LIB::DFSDM_CHANNEL_DOMAIN::Type_Conversion::Regular,
    .dma = ST_LIB::DFSDM_CHANNEL_DOMAIN::Dma::Disable,
    .fast = ST_LIB::DFSDM_CHANNEL_DOMAIN::Fast_Conversion::Disable,
    .rsync = ST_LIB::DFSDM_CHANNEL_DOMAIN::Sync_Conversion::Independent,
    .rcont = ST_LIB::DFSDM_CHANNEL_DOMAIN::Regular_Mode::Single,
    .jscan = ST_LIB::DFSDM_CHANNEL_DOMAIN::Injected_Mode::Scan,
    .overrun = ST_LIB::DFSDM_CHANNEL_DOMAIN::Overrun::Disable,
    .watchdog = ST_LIB::DFSDM_CHANNEL_DOMAIN::Analog_Watchdog::Disable,
};

// DFSDM Channel with Sinc5 filter (stricter oversampling)
constexpr ST_LIB::DFSDM_CHANNEL_DOMAIN::Config_Filter dfsdm_filter_sinc5_cfg{
    .filter = 1,
    .trigger_conv = ST_LIB::DFSDM_CHANNEL_DOMAIN::Trigger_Timer_Source::Unused,
    .filter_type = ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc5,
    .oversampling = 64,
    .integrator = 1,
    .type_conv = ST_LIB::DFSDM_CHANNEL_DOMAIN::Type_Conversion::Injected,
    .dma = ST_LIB::DFSDM_CHANNEL_DOMAIN::Dma::Disable,
    .fast = ST_LIB::DFSDM_CHANNEL_DOMAIN::Fast_Conversion::Disable,
    .rsync = ST_LIB::DFSDM_CHANNEL_DOMAIN::Sync_Conversion::Independent,
    .rcont = ST_LIB::DFSDM_CHANNEL_DOMAIN::Regular_Mode::Continuous,
    .jscan = ST_LIB::DFSDM_CHANNEL_DOMAIN::Injected_Mode::Single,
    .overrun = ST_LIB::DFSDM_CHANNEL_DOMAIN::Overrun::Enable,
    .watchdog = ST_LIB::DFSDM_CHANNEL_DOMAIN::Analog_Watchdog::Enable,
    .watchdog_low_threshold = 0x00000000,
    .watchdog_high_threshold = 0x7FFFFFFF,
};

// Test DFSDM Channel constructor with PE4 pin
constexpr ST_LIB::DFSDM_CHANNEL_DOMAIN::DFSDM_CHANNEL dfsdm_ch_pe4{
    ST_LIB::PE4,
    dfsdm_channel_cfg,
    dfsdm_filter_cfg,
    128
};

// Test DFSDM Channel constructor with PC0 pin
constexpr ST_LIB::DFSDM_CHANNEL_DOMAIN::DFSDM_CHANNEL dfsdm_ch_pc0{
    ST_LIB::PC0,
    dfsdm_channel_cfg,
    dfsdm_filter_sinc5_cfg,
    256
};

// Compile-time validation tests
static_assert(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
                  ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::FastSinc, 32),
              "FastSinc with oversampling 32 should be valid");

static_assert(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
                  ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc4, 215),
              "Sinc4 with oversampling 215 should be valid");

static_assert(!ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
                  ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc4, 216),
              "Sinc4 with oversampling 216 should be invalid (exceeds max)");

static_assert(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PE4) == 3,
              "PE4 should map to channel 3");

static_assert(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PC0) == 4,
              "PC0 should map to channel 4");

} // namespace

class DFSDMTest : public ::testing::Test {
protected:
    void SetUp() override {
        ST_LIB::TestErrorHandler::reset();
        ST_LIB::TestErrorHandler::set_fail_on_error(true);
    }

    void TearDown() override { ST_LIB::TestErrorHandler::set_fail_on_error(false); }
};

TEST_F(DFSDMTest, ChannelConfigurationIsValidAtCompileTime) {
    // This test validates that DFSDM channel configuration passes compile-time checks
    // If we reach here, compile-time assertions have passed
    EXPECT_TRUE(true);
}

TEST_F(DFSDMTest, FilterTypeValidationEnforcesOversamplingLimits) {
    // FastSinc can use up to 1024 oversampling
    EXPECT_TRUE(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::FastSinc, 1024));

    // FastSinc cannot exceed 1024
    EXPECT_FALSE(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::FastSinc, 1025));

    // Sinc5 can use up to 73 oversampling
    EXPECT_TRUE(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc5, 73));

    // Sinc5 cannot exceed 73
    EXPECT_FALSE(ST_LIB::DFSDM_CHANNEL_DOMAIN::is_correct_oversampling(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc5, 74));
}

TEST_F(DFSDMTest, PinToChannelMappingIsCorrect) {
    // Verify pin to channel mapping
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PE4), 3);
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PC0), 4);
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PC1), 0);
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PC3), 1);
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PC5), 2);
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::get_channel(ST_LIB::PB1), 1);
}

TEST_F(DFSDMTest, DefinesMaximumInstanceCount) {
    // DFSDM supports up to 8 channels
    EXPECT_EQ(ST_LIB::DFSDM_CHANNEL_DOMAIN::max_instances, 8);
}

TEST_F(DFSDMTest, AlternateFunctionMappingForDifferentPins) {
    // Port B pins 1, 10, 12, 14 use AF6
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::dfsdm_channel_af(ST_LIB::PB1),
        ST_LIB::GPIODomain::AlternateFunction::AF6
    );

    // Port C pin 7 uses AF4
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::dfsdm_channel_af(ST_LIB::PC7),
        ST_LIB::GPIODomain::AlternateFunction::AF4
    );

    // Port B pin 6 uses AF11
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::dfsdm_channel_af(ST_LIB::PB6),
        ST_LIB::GPIODomain::AlternateFunction::AF11
    );

    // Other pins default to AF3
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::dfsdm_channel_af(ST_LIB::PE4),
        ST_LIB::GPIODomain::AlternateFunction::AF3
    );
}

TEST_F(DFSDMTest, DMAPeripheralMappingForFilters) {
    // Each filter maps to a specific DMA peripheral
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::get_dma_peripheral(0),
        ST_LIB::DMA_Domain::Peripheral::dfsdm_filter0
    );
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::get_dma_peripheral(1),
        ST_LIB::DMA_Domain::Peripheral::dfsdm_filter1
    );
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::get_dma_peripheral(2),
        ST_LIB::DMA_Domain::Peripheral::dfsdm_filter2
    );
    EXPECT_EQ(
        ST_LIB::DFSDM_CHANNEL_DOMAIN::get_dma_peripheral(3),
        ST_LIB::DMA_Domain::Peripheral::dfsdm_filter3
    );
}

TEST_F(DFSDMTest, DFSDMChannelConstructorValidatesConfiguration) {
    // Test that DFSDM_CHANNEL constructors with valid configurations compile
    // If we reach here, compile-time validation has passed
    EXPECT_EQ(dfsdm_ch_pe4.channel, 3);
    EXPECT_EQ(dfsdm_ch_pe4.buffer_size, 128);
    EXPECT_EQ(dfsdm_ch_pc0.channel, 4);
    EXPECT_EQ(dfsdm_ch_pc0.buffer_size, 256);
}

TEST_F(DFSDMTest, DFSDMMultipleChannelsWithDifferentFilters) {
    // Validate that multiple channels with different filter types can coexist
    EXPECT_NE(dfsdm_ch_pe4.config_filter.filter_type, dfsdm_ch_pc0.config_filter.filter_type);
    EXPECT_EQ(dfsdm_ch_pe4.config_filter.filter_type, ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::FastSinc);
    EXPECT_EQ(dfsdm_ch_pc0.config_filter.filter_type, ST_LIB::DFSDM_CHANNEL_DOMAIN::Filter_Type::Sinc5);
}

TEST_F(DFSDMTest, DFSDMChannelConfigurationDifferentModes) {
    // Validate different channel configurations
    EXPECT_EQ(dfsdm_ch_pe4.config_filter.type_conv, ST_LIB::DFSDM_CHANNEL_DOMAIN::Type_Conversion::Regular);
    EXPECT_EQ(dfsdm_ch_pc0.config_filter.type_conv, ST_LIB::DFSDM_CHANNEL_DOMAIN::Type_Conversion::Injected);
    EXPECT_NE(dfsdm_ch_pe4.config_filter.rcont, dfsdm_ch_pc0.config_filter.rcont);
}
