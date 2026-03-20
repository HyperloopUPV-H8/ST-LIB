#include <gtest/gtest.h>

#include "HALAL/Services/Time/TimerWrapper.hpp"
#include "ST-LIB_LOW/Sensors/EncoderSensor/NewEncoderSensor.hpp"

namespace ST_LIB::TestErrorHandler {
void reset();
void set_fail_on_error(bool enabled);
extern int call_count;
} // namespace ST_LIB::TestErrorHandler

namespace {

constexpr ST_LIB::TimerDomain::Timer encoder_timer_decl{
    ST_LIB::TimerRequest::GeneralPurpose32bit_2,
    ST_LIB::TimerDomain::EMPTY_TIMER_NAME,
    ST_LIB::TimerPin{
        .af = ST_LIB::TimerAF::Encoder,
        .pin = ST_LIB::PA0,
        .channel = ST_LIB::TimerChannel::CHANNEL_1,
    },
    ST_LIB::TimerPin{
        .af = ST_LIB::TimerAF::Encoder,
        .pin = ST_LIB::PA1,
        .channel = ST_LIB::TimerChannel::CHANNEL_2,
    },
};

struct MockEncoder {
    uint32_t counter = 1234;
    uint32_t initial_counter = 1234;
    bool direction = true;
    int turn_on_calls = 0;
    int turn_off_calls = 0;
    int reset_calls = 0;

    void turn_on() { ++turn_on_calls; }
    void turn_off() { ++turn_off_calls; }
    void reset() {
        ++reset_calls;
        counter = initial_counter;
    }
    uint32_t get_counter() { return counter; }
    bool get_direction() { return direction; }
    uint32_t get_initial_counter_value() { return initial_counter; }
};

using MockSensor = ST_LIB::EncoderSensor<MockEncoder, 4>;

} // namespace

class EncoderTest : public ::testing::Test {
protected:
    TIM_HandleTypeDef hal_tim{};
    ST_LIB::TimerDomain::Instance instance{};
    ST_LIB::TimerWrapper<encoder_timer_decl> wrapper{};

    void SetUp() override {
        ST_LIB::TestErrorHandler::reset();

        TIM2_BASE->CNT = 0U;
        TIM2_BASE->ARR = 0U;
        TIM2_BASE->PSC = 0U;
        TIM2_BASE->CR1 = 0U;

        hal_tim = {};
        hal_tim.Instance = TIM2_BASE;
        hal_tim.State = HAL_TIM_STATE_READY;

        instance.tim = TIM2_BASE;
        instance.hal_tim = &hal_tim;
        instance.timer_idx = 0U;

        wrapper = ST_LIB::TimerWrapper<encoder_timer_decl>(&instance);
        ST_LIB::Encoder<encoder_timer_decl>::init(&wrapper);
        ST_LIB::Encoder<encoder_timer_decl>::turn_off();
    }
};

TEST_F(EncoderTest, ResetUsesConfiguredInitialCounterValue) {
    TIM2_BASE->CNT = 17U;

    ST_LIB::Encoder<encoder_timer_decl>::reset();

    EXPECT_EQ(TIM2_BASE->ARR, UINT32_MAX);
    EXPECT_EQ(TIM2_BASE->CNT, ST_LIB::Encoder<encoder_timer_decl>::get_initial_counter_value());
}

TEST_F(EncoderTest, TurnOffKeepsTryingIfHALStopFails) {
    ST_LIB::TestErrorHandler::set_fail_on_error(false);
    ST_LIB::Encoder<encoder_timer_decl>::turn_on();

    instance.hal_tim = nullptr;

    ST_LIB::Encoder<encoder_timer_decl>::turn_off();
    ST_LIB::Encoder<encoder_timer_decl>::turn_off();

    EXPECT_EQ(ST_LIB::TestErrorHandler::call_count, 2);
}

TEST(EncoderSensorTest, ReadTreatsEncoderInitialCounterAsZeroPosition) {
    MockEncoder encoder{};
    double position = -1.0;
    double speed = -1.0;
    double acceleration = -1.0;
    MockSensor::Direction direction = MockSensor::BACKWARDS;

    MockSensor sensor(encoder, 0.5, 0.1, &direction, &position, &speed, &acceleration);

    sensor.read();

    EXPECT_DOUBLE_EQ(position, 0.0);
    EXPECT_DOUBLE_EQ(speed, 0.0);
    EXPECT_DOUBLE_EQ(acceleration, 0.0);
    EXPECT_EQ(direction, MockSensor::FORWARD);
}

TEST(EncoderSensorTest, ResetForwardsToEncoderAndClearsHistory) {
    MockEncoder encoder{};
    encoder.counter = 1240U;

    double position = 0.0;
    double speed = 0.0;
    double acceleration = 0.0;
    MockSensor::Direction direction = MockSensor::BACKWARDS;

    MockSensor sensor(encoder, 1.0, 1.0, &direction, &position, &speed, &acceleration);

    sensor.read();
    ASSERT_NE(position, 0.0);

    sensor.reset();
    sensor.read();

    EXPECT_EQ(encoder.reset_calls, 1);
    EXPECT_DOUBLE_EQ(position, 0.0);
    EXPECT_DOUBLE_EQ(speed, 0.0);
    EXPECT_DOUBLE_EQ(acceleration, 0.0);
}
