#include <gtest/gtest.h>

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "HALAL/Services/InfoWarning/InfoWarning.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionEngine.hpp"
#include "ST-LIB_HIGH/Protections/Rules.hpp"
#include "ST-LIB_HIGH/Protections/SampleSource.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

namespace ST_LIB::TestErrorHandler {
void set_fail_on_error(bool enabled);
}

namespace {

namespace TestErrorHandler = ST_LIB::TestErrorHandler;

static_assert(Protections::ReadableSampleSource<SampleSource<float>>);
static_assert(!Protections::ReadableSampleSource<int>);
static_assert(Protections::FloatingSample<float>);
static_assert(!Protections::FloatingSample<int>);
static_assert(Protections::ComparableSample<float>);
static_assert(!Protections::ComparableSample<const char*>);

class RecordingSink final : public Diagnostics::DiagnosticSink {
public:
    explicit RecordingSink(size_t failures_before_success = 0)
        : failures_before_success(failures_before_success) {}

    bool publish(const Diagnostics::DiagnosticRecord& record) override {
        publish_calls++;
        records.push_back(record);
        if (publish_calls <= failures_before_success) {
            return false;
        }
        return true;
    }

    size_t failures_before_success;
    size_t publish_calls{0};
    vector<Diagnostics::DiagnosticRecord> records{};
};

class DummyStateMachine final : public IStateMachine {
public:
    void check_transitions() override {}

    void force_change_state(size_t state) override {
        force_change_calls++;
        current_state_id = state;
    }

    size_t get_current_state_id() const override { return current_state_id; }

    size_t force_change_calls{0};
    size_t current_state_id{0};

protected:
    void enter() override {}
    void exit() override {}
    void start() override {}
};

class CountingBroadcaster final : public FaultBroadcaster {
public:
    bool broadcast_fault() override {
        calls++;
        return true;
    }

    size_t calls{0};
};

class DiagnosticsHubTest : public ::testing::Test {
protected:
    void SetUp() override {
        Diagnostics::Hub::clear_for_testing();
        ProtectionEngine::clear_for_testing();
        FaultController::clear_broadcasters_for_testing();
    }
};

TEST_F(DiagnosticsHubTest, KeepsLocalHistoryWhenNoSinksAreRegistered) {
    Diagnostics::DiagnosticRecord record{};
    record.severity = Diagnostics::Severity::WARNING;
    record.category = Diagnostics::Category::RUNTIME_WARNING;
    snprintf(record.origin, sizeof(record.origin), "test");
    snprintf(record.payload.runtime.message, sizeof(record.payload.runtime.message), "local only");
    Diagnostics::Hub::publish(record);

    EXPECT_EQ(Diagnostics::Hub::history_size_for_testing(), 1u);
    EXPECT_EQ(Diagnostics::Hub::pending_size_for_testing(), 0u);
}

TEST_F(DiagnosticsHubTest, RetriesOnlyTheSinkThatFailed) {
    auto stable_sink_result = Diagnostics::Hub::emplace_sink<RecordingSink>();
    auto flaky_sink_result = Diagnostics::Hub::emplace_sink<RecordingSink>(1);
    ASSERT_TRUE(stable_sink_result.has_value());
    ASSERT_TRUE(flaky_sink_result.has_value());
    auto* stable_sink = *stable_sink_result;
    auto* flaky_sink = *flaky_sink_result;

    Diagnostics::DiagnosticRecord record{};
    record.severity = Diagnostics::Severity::WARNING;
    record.category = Diagnostics::Category::RUNTIME_WARNING;
    snprintf(record.origin, sizeof(record.origin), "test");
    snprintf(record.payload.runtime.message, sizeof(record.payload.runtime.message), "retry me");
    Diagnostics::Hub::publish(record);

    Diagnostics::Hub::flush();
    EXPECT_EQ(stable_sink->publish_calls, 1u);
    EXPECT_EQ(flaky_sink->publish_calls, 1u);
    EXPECT_EQ(Diagnostics::Hub::pending_size_for_testing(), 1u);

    Diagnostics::Hub::flush();
    EXPECT_EQ(stable_sink->publish_calls, 1u);
    EXPECT_EQ(flaky_sink->publish_calls, 2u);
    EXPECT_EQ(Diagnostics::Hub::pending_size_for_testing(), 0u);
}

TEST_F(DiagnosticsHubTest, PendingQueueIsBoundedWhenASinkNeverDelivers) {
    auto stuck_sink_result =
        Diagnostics::Hub::emplace_sink<RecordingSink>(std::numeric_limits<size_t>::max());
    ASSERT_TRUE(stuck_sink_result.has_value());

    for (size_t record_index = 0; record_index < Diagnostics::Config::pending_capacity + 5;
         record_index++) {
        Diagnostics::DiagnosticRecord record{};
        record.severity = Diagnostics::Severity::WARNING;
        record.category = Diagnostics::Category::RUNTIME_WARNING;
        snprintf(record.origin, sizeof(record.origin), "test");
        snprintf(record.payload.runtime.message, sizeof(record.payload.runtime.message), "event %zu", record_index);
        Diagnostics::Hub::publish(record);
    }

    EXPECT_EQ(Diagnostics::Hub::pending_size_for_testing(), Diagnostics::Config::pending_capacity);
}

TEST_F(DiagnosticsHubTest, FaultControllerTransitionsOnlyOnce) {
    DummyStateMachine machine{};

    FaultController::link_state_machine(machine, 4);
    auto broadcaster_result = FaultController::emplace_broadcaster<CountingBroadcaster>();
    ASSERT_TRUE(broadcaster_result.has_value());
    auto* broadcaster = *broadcaster_result;

    FaultController::enter_fault();
    FaultController::enter_fault();

    EXPECT_EQ(machine.current_state_id, 4u);
    EXPECT_EQ(machine.force_change_calls, 1u);
    EXPECT_EQ(broadcaster->calls, 1u);
}

TEST_F(DiagnosticsHubTest, ProtectionEngineEvaluatesRulesAndPublishesSnapshots) {
    DummyStateMachine machine{};
    float monitored_value = 2.0f;
    SampleSource<float> source(monitored_value);

    auto sink_result = Diagnostics::Hub::emplace_sink<RecordingSink>();
    ASSERT_TRUE(sink_result.has_value());
    auto* sink = *sink_result;
    FaultController::link_state_machine(machine, 7);
    auto protection = ProtectionEngine::create_protection("monitored_value", source);
    ASSERT_TRUE(protection.has_value());
    ASSERT_TRUE(protection->add_rule(Protections::Rules::below(1.0f, 1.5f)).has_value());

    ProtectionEngine::initialize();
    monitored_value = 0.5f;
    ProtectionEngine::evaluate();
    Diagnostics::Hub::flush();

    ASSERT_FALSE(sink->records.empty());
    EXPECT_EQ(machine.current_state_id, 7u);
    EXPECT_EQ(sink->records.front().category, Diagnostics::Category::PROTECTION_EVENT);
    EXPECT_EQ(
        sink->records.front().payload.protection.state,
        Protections::RuleState::FAULT
    );
    EXPECT_EQ(
        sink->records.front().payload.protection.rule_kind,
        Protections::RuleKind::BELOW
    );
}

TEST_F(DiagnosticsHubTest, ErrorHandlerPublishesAndEntersFault) {
    DummyStateMachine machine{};

    auto sink_result = Diagnostics::Hub::emplace_sink<RecordingSink>();
    ASSERT_TRUE(sink_result.has_value());
    auto* sink = *sink_result;
    FaultController::link_state_machine(machine, 5);
    TestErrorHandler::set_fail_on_error(false);

    ErrorHandler("runtime failure %d", 12);
    Diagnostics::Hub::flush();

    ASSERT_FALSE(sink->records.empty());
    EXPECT_EQ(machine.current_state_id, 5u);
    EXPECT_EQ(sink->records.front().category, Diagnostics::Category::RUNTIME_ERROR);
    EXPECT_EQ(sink->records.front().severity, Diagnostics::Severity::FAULT);
}

TEST_F(DiagnosticsHubTest, WarningDoesNotEnterFault) {
    DummyStateMachine machine{};

    auto sink_result = Diagnostics::Hub::emplace_sink<RecordingSink>();
    ASSERT_TRUE(sink_result.has_value());
    auto* sink = *sink_result;
    FaultController::link_state_machine(machine, 9);

    WARNING("runtime warning %d", 3);
    Diagnostics::Hub::flush();

    ASSERT_FALSE(sink->records.empty());
    EXPECT_EQ(machine.current_state_id, 0u);
    EXPECT_EQ(sink->records.front().category, Diagnostics::Category::RUNTIME_WARNING);
    EXPECT_EQ(sink->records.front().severity, Diagnostics::Severity::WARNING);
}

TEST_F(DiagnosticsHubTest, RejectsInvalidRuleConfigurationsWithoutGlobalSideEffects) {
    auto invalid_rule = Protections::Rules::below(1.0f, 0.5f);
    EXPECT_FALSE(invalid_rule.has_value());
    EXPECT_EQ(invalid_rule.error(), Protections::RuleConfigError::INVALID_WARNING_THRESHOLD);
}

} // namespace
