#include <gtest/gtest.h>

#include "HALAL/Services/Time/Scheduler.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"

namespace TestBaseline {
    Protections::ProtectionEvaluation eval_bus(float val);
    Protections::ProtectionEvaluation eval_time(float val);
    Protections::ProtectionEvaluation eval_current(float val);
    Protections::ProtectionEvaluation eval_range(float val);
}

namespace TestOptimized {
    Protections::ProtectionEvaluation eval_bus(float val);
    Protections::ProtectionEvaluation eval_time(float val);
    Protections::ProtectionEvaluation eval_current(float val);
    Protections::ProtectionEvaluation eval_range(float val);
}

static void assert_same_evaluation(
    const Protections::ProtectionEvaluation& base,
    const Protections::ProtectionEvaluation& opt,
    const char* name
) {
    SCOPED_TRACE(name);
    EXPECT_EQ(base.aggregated_state, opt.aggregated_state) << "aggregated_state";
    EXPECT_EQ(base.has_active_fault,   opt.has_active_fault)   << "has_active_fault";
    EXPECT_EQ(base.active_fault_edge,  opt.active_fault_edge)  << "active_fault_edge";
    EXPECT_EQ(base.event_count,        opt.event_count)        << "event_count";
    for (size_t i = 0; i < base.event_count; ++i) {
        SCOPED_TRACE("event " + std::to_string(i));
        EXPECT_EQ(base.events[i].state, opt.events[i].state);
        EXPECT_EQ(base.events[i].edge,  opt.events[i].edge);
    }
    if (base.has_active_fault) {
        EXPECT_EQ(base.active_fault_snapshot.kind, opt.active_fault_snapshot.kind);
    }
}

class ProtectionCompareTest : public ::testing::Test {
protected:
    void SetUp() override {
        Scheduler::global_tick_us_ = 0;
        Scheduler_global_timer = nullptr;
    }
};

TEST_F(ProtectionCompareTest, BusVoltageNormal) {
    auto base = TestBaseline::eval_bus(500.0f);
    auto opt  = TestOptimized::eval_bus(500.0f);
    assert_same_evaluation(base, opt, "bus 500V");
}
TEST_F(ProtectionCompareTest, BusVoltageBelowWarning) {
    auto base = TestBaseline::eval_bus(340.0f);
    auto opt  = TestOptimized::eval_bus(340.0f);
    assert_same_evaluation(base, opt, "bus 340V");
}
TEST_F(ProtectionCompareTest, BusVoltageBelowFault) {
    auto base = TestBaseline::eval_bus(310.0f);
    auto opt  = TestOptimized::eval_bus(310.0f);
    assert_same_evaluation(base, opt, "bus 310V");
}
TEST_F(ProtectionCompareTest, BusVoltageAboveFault) {
    auto base = TestBaseline::eval_bus(610.0f);
    auto opt  = TestOptimized::eval_bus(610.0f);
    assert_same_evaluation(base, opt, "bus 610V");
}
TEST_F(ProtectionCompareTest, CurrentNormal) {
    auto base = TestBaseline::eval_current(85.0f);
    auto opt  = TestOptimized::eval_current(85.0f);
    assert_same_evaluation(base, opt, "current 85A");
}
TEST_F(ProtectionCompareTest, CurrentWarning) {
    auto base = TestBaseline::eval_current(100.0f);
    auto opt  = TestOptimized::eval_current(100.0f);
    assert_same_evaluation(base, opt, "current 100A");
}
TEST_F(ProtectionCompareTest, CurrentFault) {
    auto base = TestBaseline::eval_current(115.0f);
    auto opt  = TestOptimized::eval_current(115.0f);
    assert_same_evaluation(base, opt, "current 115A");
}
TEST_F(ProtectionCompareTest, TimeAccumDoesNotFireInstantly) {
    Scheduler::global_tick_us_ = 0;
    auto base = TestBaseline::eval_time(12.0f);
    auto opt  = TestOptimized::eval_time(12.0f);
    assert_same_evaluation(base, opt, "time_accum t=0");
}
TEST_F(ProtectionCompareTest, TimeAccumFiresAfterWindow) {
    Scheduler::global_tick_us_ = 0;
    TestBaseline::eval_time(12.0f);
    TestOptimized::eval_time(12.0f);
    Scheduler::global_tick_us_ = 500;
    auto base = TestBaseline::eval_time(12.0f);
    auto opt  = TestOptimized::eval_time(12.0f);
    assert_same_evaluation(base, opt, "time_accum t=500");
    Scheduler::global_tick_us_ = 1100;
    base = TestBaseline::eval_time(12.0f);
    opt  = TestOptimized::eval_time(12.0f);
    assert_same_evaluation(base, opt, "time_accum t=1100");
}
TEST_F(ProtectionCompareTest, TimeAccumResetsWhenBelowThreshold) {
    Scheduler::global_tick_us_ = 0;
    TestBaseline::eval_time(12.0f);
    TestOptimized::eval_time(12.0f);
    Scheduler::global_tick_us_ = 800;
    auto base = TestBaseline::eval_time(5.0f);
    auto opt  = TestOptimized::eval_time(5.0f);
    assert_same_evaluation(base, opt, "time_accum below threshold");
    Scheduler::global_tick_us_ = 1800;
    base = TestBaseline::eval_time(5.0f);
    opt  = TestOptimized::eval_time(5.0f);
    assert_same_evaluation(base, opt, "time_accum still NORMAL");
}
TEST_F(ProtectionCompareTest, RangeNormal) {
    auto base = TestBaseline::eval_range(45.0f);
    auto opt  = TestOptimized::eval_range(45.0f);
    assert_same_evaluation(base, opt, "range 45C");
}
TEST_F(ProtectionCompareTest, RangeBelowFault) {
    auto base = TestBaseline::eval_range(-50.0f);
    auto opt  = TestOptimized::eval_range(-50.0f);
    assert_same_evaluation(base, opt, "range -50C");
}
TEST_F(ProtectionCompareTest, RangeAboveFault) {
    auto base = TestBaseline::eval_range(160.0f);
    auto opt  = TestOptimized::eval_range(160.0f);
    assert_same_evaluation(base, opt, "range 160C");
}
TEST_F(ProtectionCompareTest, EdgeTransitionNormalToWarning) {
    TestBaseline::eval_bus(500.0f);
    TestOptimized::eval_bus(500.0f);
    auto base = TestBaseline::eval_bus(340.0f);
    auto opt  = TestOptimized::eval_bus(340.0f);
    assert_same_evaluation(base, opt, "NORMAL→WARNING");
}
TEST_F(ProtectionCompareTest, EdgeTransitionWarningToFault) {
    TestBaseline::eval_bus(340.0f);
    TestOptimized::eval_bus(340.0f);
    auto base = TestBaseline::eval_bus(310.0f);
    auto opt  = TestOptimized::eval_bus(310.0f);
    assert_same_evaluation(base, opt, "WARNING→FAULT");
}
TEST_F(ProtectionCompareTest, EdgeTransitionFaultToRecovered) {
    TestBaseline::eval_bus(310.0f);
    TestOptimized::eval_bus(310.0f);
    auto base = TestBaseline::eval_bus(500.0f);
    auto opt  = TestOptimized::eval_bus(500.0f);
    assert_same_evaluation(base, opt, "FAULT→RECOVERED");
}
