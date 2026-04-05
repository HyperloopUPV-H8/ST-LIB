#pragma once

#include <cmath>

#include "C++Utilities/CppUtils.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionErrors.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"
#include "ST-LIB_HIGH/Protections/Rules.hpp"
#include "ST-LIB_HIGH/Protections/SampleSource.hpp"

namespace Protections {

namespace detail {

template <typename T> constexpr T zero_value() { return static_cast<T>(0); }

template <typename T> constexpr T absolute_value(T value) {
    if constexpr (std::is_floating_point_v<T>) {
        return static_cast<T>(std::fabs(value));
    } else if constexpr (std::is_signed_v<T>) {
        return value < 0 ? static_cast<T>(-value) : value;
    } else {
        return value;
    }
}

template <typename T> constexpr bool is_below(T sample, T threshold) { return sample < threshold; }

template <typename T> constexpr bool is_above(T sample, T threshold) { return sample > threshold; }

template <typename T> constexpr bool is_equal_to(T lhs, T rhs) { return lhs == rhs; }

template <typename T> constexpr bool is_not_equal_to(T lhs, T rhs) { return !is_equal_to(lhs, rhs); }

} // namespace detail

class RuleStateTracker {
public:
    constexpr RuleState previous_state() const { return last_state; }

    constexpr RuleEdge advance(RuleState current_state) {
        RuleEdge edge = RuleEdge::NONE;
        if (current_state == RuleState::FAULT && last_state != RuleState::FAULT) {
            edge = RuleEdge::FAULT_RAISED;
        } else if (current_state == RuleState::WARNING && last_state != RuleState::WARNING) {
            edge = RuleEdge::WARNING_RAISED;
        } else if (current_state == RuleState::NORMAL && last_state != RuleState::NORMAL) {
            edge = RuleEdge::RECOVERED;
        }

        last_state = current_state;
        return edge;
    }

private:
    RuleState last_state{RuleState::NORMAL};
};

class RuleSnapshotBuilder {
public:
    template <ProtectionSample T>
    static RuleSnapshot single_threshold(
        RuleKind kind,
        T observed,
        RuleState current_state,
        RuleEdge edge,
        RuleState previous_state,
        T fault_threshold,
        optional<T> warning_threshold,
        float time_window_s = 0.0f,
        float sample_rate_hz = 0.0f
    ) {
        RuleSnapshot snapshot{};
        snapshot.kind = kind;
        snapshot.sample_encoding = sample_encoding_for<T>();
        snapshot.observed_value = to_numeric_value(observed);
        snapshot.time_window_s = time_window_s;
        snapshot.sample_rate_hz = sample_rate_hz;
        snapshot.uses_warning_threshold = should_use_warning_threshold(
            current_state,
            edge,
            previous_state,
            warning_threshold.has_value()
        );

        snapshot.threshold_a = to_numeric_value(
            snapshot.uses_warning_threshold ? warning_threshold.value_or(fault_threshold)
                                            : fault_threshold
        );
        return snapshot;
    }

    template <ProtectionSample T>
    static RuleSnapshot range(
        T observed,
        RuleState current_state,
        RuleEdge edge,
        RuleState previous_state,
        const RangeRuleConfig<T>& config
    ) {
        RuleSnapshot snapshot{};
        snapshot.kind = RuleKind::RANGE;
        snapshot.sample_encoding = sample_encoding_for<T>();
        snapshot.observed_value = to_numeric_value(observed);
        snapshot.has_threshold_b = true;
        snapshot.uses_warning_threshold = should_use_warning_threshold(
            current_state,
            edge,
            previous_state,
            config.low_warning.has_value() && config.high_warning.has_value()
        );
        snapshot.threshold_a = to_numeric_value(
            snapshot.uses_warning_threshold ? config.low_warning.value_or(config.low_fault)
                                            : config.low_fault
        );
        snapshot.threshold_b = to_numeric_value(
            snapshot.uses_warning_threshold ? config.high_warning.value_or(config.high_fault)
                                            : config.high_fault
        );
        return snapshot;
    }

private:
    static constexpr bool should_use_warning_threshold(
        RuleState current_state,
        RuleEdge edge,
        RuleState previous_state,
        bool has_warning_threshold
    ) {
        return has_warning_threshold &&
               (current_state == RuleState::WARNING ||
                (edge == RuleEdge::RECOVERED && previous_state == RuleState::WARNING));
    }
};

template <ComparableSample T> struct BelowEvaluator {
    static constexpr RuleState compute(const T& sample, const BelowRuleConfig<T>& config) {
        if (detail::is_below(sample, config.fault_threshold)) {
            return RuleState::FAULT;
        }
        if (config.warning_threshold.has_value() &&
            detail::is_below(sample, config.warning_threshold.value())) {
            return RuleState::WARNING;
        }
        return RuleState::NORMAL;
    }
};

template <ComparableSample T> struct AboveEvaluator {
    static constexpr RuleState compute(const T& sample, const AboveRuleConfig<T>& config) {
        if (detail::is_above(sample, config.fault_threshold)) {
            return RuleState::FAULT;
        }
        if (config.warning_threshold.has_value() &&
            detail::is_above(sample, config.warning_threshold.value())) {
            return RuleState::WARNING;
        }
        return RuleState::NORMAL;
    }
};

template <ComparableSample T> struct RangeEvaluator {
    static constexpr RuleState compute(const T& sample, const RangeRuleConfig<T>& config) {
        if (detail::is_below(sample, config.low_fault) ||
            detail::is_above(sample, config.high_fault)) {
            return RuleState::FAULT;
        }
        if (config.low_warning.has_value() && config.high_warning.has_value() &&
            (detail::is_below(sample, config.low_warning.value()) ||
             detail::is_above(sample, config.high_warning.value()))) {
            return RuleState::WARNING;
        }
        return RuleState::NORMAL;
    }
};

template <EqualityComparableSample T> struct EqualsEvaluator {
    static constexpr RuleState compute(const T& sample, const EqualsRuleConfig<T>& config) {
        return detail::is_equal_to(sample, config.expected) ? RuleState::FAULT
                                                            : RuleState::NORMAL;
    }
};

template <EqualityComparableSample T> struct NotEqualsEvaluator {
    static constexpr RuleState compute(const T& sample, const NotEqualsRuleConfig<T>& config) {
        return detail::is_not_equal_to(sample, config.expected) ? RuleState::FAULT
                                                                : RuleState::NORMAL;
    }
};

template <FloatingSample T> struct TimeAccumulationEvaluator {
    static RuleState compute(
        const T& sample,
        const TimeAccumulationRuleConfig<T>& config,
        array<T, Config::max_time_accumulation_samples>& accumulation_window,
        size_t configured_window_samples,
        size_t& window_fill_count,
        size_t& window_index,
        T& rolling_sum,
        T& average_value
    ) {
        const T magnitude = detail::absolute_value(sample);
        const size_t window_samples = configured_window_samples == 0 ? 1 : configured_window_samples;

        if (window_fill_count < window_samples) {
            rolling_sum += magnitude;
            accumulation_window[window_fill_count] = magnitude;
            window_fill_count++;
            average_value = static_cast<T>(rolling_sum / static_cast<T>(window_fill_count));
            return RuleState::NORMAL;
        }

        rolling_sum -= accumulation_window[window_index];
        accumulation_window[window_index] = magnitude;
        rolling_sum += magnitude;
        window_index = (window_index + 1) % window_samples;
        average_value = static_cast<T>(rolling_sum / static_cast<T>(window_samples));

        if (detail::is_above(average_value, config.fault_threshold)) {
            return RuleState::FAULT;
        }
        if (config.warning_threshold.has_value() &&
            detail::is_above(average_value, config.warning_threshold.value())) {
            return RuleState::WARNING;
        }
        return RuleState::NORMAL;
    }
};

template <ComparableSample T> struct BelowRule {
    BelowRuleConfig<T> config{};
    RuleStateTracker tracker{};

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = BelowEvaluator<T>::compute(sample, config);
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot = RuleSnapshotBuilder::single_threshold(
                RuleKind::BELOW,
                sample,
                state,
                edge,
                previous_state,
                config.fault_threshold,
                config.warning_threshold
            ),
        };
    }
};

template <ComparableSample T> struct AboveRule {
    AboveRuleConfig<T> config{};
    RuleStateTracker tracker{};

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = AboveEvaluator<T>::compute(sample, config);
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot = RuleSnapshotBuilder::single_threshold(
                RuleKind::ABOVE,
                sample,
                state,
                edge,
                previous_state,
                config.fault_threshold,
                config.warning_threshold
            ),
        };
    }
};

template <ComparableSample T> struct RangeRule {
    RangeRuleConfig<T> config{};
    RuleStateTracker tracker{};

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = RangeEvaluator<T>::compute(sample, config);
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot =
                RuleSnapshotBuilder::range(sample, state, edge, previous_state, config),
        };
    }
};

template <EqualityComparableSample T> struct EqualsRule {
    EqualsRuleConfig<T> config{};
    RuleStateTracker tracker{};

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = EqualsEvaluator<T>::compute(sample, config);
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot = RuleSnapshotBuilder::single_threshold(
                RuleKind::EQUALS,
                sample,
                state,
                edge,
                previous_state,
                config.expected,
                optional<T>{}
            ),
        };
    }
};

template <EqualityComparableSample T> struct NotEqualsRule {
    NotEqualsRuleConfig<T> config{};
    RuleStateTracker tracker{};

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = NotEqualsEvaluator<T>::compute(sample, config);
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot = RuleSnapshotBuilder::single_threshold(
                RuleKind::NOT_EQUALS,
                sample,
                state,
                edge,
                previous_state,
                config.expected,
                optional<T>{}
            ),
        };
    }
};

template <FloatingSample T> struct TimeAccumulationRule {
    explicit TimeAccumulationRule(TimeAccumulationRuleConfig<T> config) : config(config) {
        configured_window_samples =
            static_cast<size_t>(std::lround(config.time_window_s * config.sample_rate_hz));
        if (configured_window_samples == 0) {
            configured_window_samples = 1;
        }
    }

    RuleEvaluation evaluate(const T& sample) {
        const RuleState previous_state = tracker.previous_state();
        const RuleState state = TimeAccumulationEvaluator<T>::compute(
            sample,
            config,
            accumulation_window,
            configured_window_samples,
            window_fill_count,
            window_index,
            rolling_sum,
            average_value
        );
        const RuleEdge edge = tracker.advance(state);
        return {
            .state = state,
            .edge = edge,
            .snapshot = RuleSnapshotBuilder::single_threshold(
                RuleKind::TIME_ACCUMULATION,
                average_value,
                state,
                edge,
                previous_state,
                config.fault_threshold,
                config.warning_threshold,
                config.time_window_s,
                config.sample_rate_hz
            ),
        };
    }

    TimeAccumulationRuleConfig<T> config{};
    RuleStateTracker tracker{};
    size_t configured_window_samples{1};
    array<T, Config::max_time_accumulation_samples> accumulation_window{};
    size_t window_fill_count{0};
    size_t window_index{0};
    T rolling_sum{detail::zero_value<T>()};
    T average_value{detail::zero_value<T>()};
};

template <ProtectionSample T, bool IsFloating = FloatingSample<T>>
struct TimeAccumulationRuleSelector {
    using type = std::monostate;
};

template <ProtectionSample T> struct TimeAccumulationRuleSelector<T, true> {
    using type = TimeAccumulationRule<T>;
};

template <ProtectionSample T>
using TimeAccumulationRuleModel = typename TimeAccumulationRuleSelector<T>::type;

template <ProtectionSample T>
using RuleModel = variant<
    BelowRule<T>,
    AboveRule<T>,
    RangeRule<T>,
    EqualsRule<T>,
    NotEqualsRule<T>,
    TimeAccumulationRuleModel<T>>;

template <ProtectionSample T> inline RuleModel<T> make_rule_model(const RuleDefinition<T>& definition) {
    return visit(
        []<typename RuleConfig>(const RuleConfig& config) -> RuleModel<T> {
            using ConfigType = std::remove_cvref_t<RuleConfig>;
            if constexpr (std::same_as<ConfigType, BelowRuleConfig<T>>) {
                return BelowRule<T>{.config = config};
            } else if constexpr (std::same_as<ConfigType, AboveRuleConfig<T>>) {
                return AboveRule<T>{.config = config};
            } else if constexpr (std::same_as<ConfigType, RangeRuleConfig<T>>) {
                return RangeRule<T>{.config = config};
            } else if constexpr (std::same_as<ConfigType, EqualsRuleConfig<T>>) {
                return EqualsRule<T>{.config = config};
            } else if constexpr (std::same_as<ConfigType, NotEqualsRuleConfig<T>>) {
                return NotEqualsRule<T>{.config = config};
            } else if constexpr (std::same_as<ConfigType, TimeAccumulationRuleConfig<T>>) {
                if constexpr (FloatingSample<T>) {
                    return TimeAccumulationRule<T>{config};
                } else {
                    std::unreachable();
                }
            } else {
                std::unreachable();
            }
        },
        definition
    );
}

template <ProtectionSample T> inline RuleEvaluation evaluate_rule(RuleModel<T>& rule, const T& sample) {
    return visit(
        [&sample](auto& concrete_rule) -> RuleEvaluation {
            using RuleType = std::remove_cvref_t<decltype(concrete_rule)>;
            if constexpr (std::same_as<RuleType, std::monostate>) {
                std::unreachable();
            } else {
                return concrete_rule.evaluate(sample);
            }
        },
        rule
    );
}

template <ProtectionSample T> class Protection {
public:
    Protection(const char* name, SampleSource<T> source) : name(name), source(source) {}

    const char* get_name() const { return name; }
    void initialize() {}

    expected<void, ProtectionError>
    add_rule(expected<RuleDefinition<T>, RuleConfigError> definition) {
        if (!definition.has_value()) {
            return unexpected(ProtectionError::INVALID_RULE_CONFIGURATION);
        }
        return add_rule(*definition);
    }

    expected<void, ProtectionError> add_rule(const RuleDefinition<T>& definition) {
        if (rule_count >= Config::max_rules_per_protection) {
            return unexpected(ProtectionError::RULE_CAPACITY_EXCEEDED);
        }

        rules[rule_count++].emplace(make_rule_model(definition));
        return {};
    }

    ProtectionEvaluation evaluate() {
        ProtectionEvaluation evaluation{};
        const T sample = source.read();

        for (size_t index = 0; index < rule_count; ++index) {
            if (!rules[index].has_value()) {
                continue;
            }

            const RuleEvaluation rule_evaluation = evaluate_rule(*rules[index], sample);
            if (rule_evaluation.edge != RuleEdge::NONE &&
                evaluation.event_count < evaluation.events.size()) {
                evaluation.events[evaluation.event_count++] = {
                    rule_evaluation.state,
                    rule_evaluation.edge,
                    rule_evaluation.snapshot,
                };
            }

            if (rule_evaluation.state == RuleState::FAULT && !evaluation.has_active_fault) {
                evaluation.has_active_fault = true;
                evaluation.active_fault_edge = rule_evaluation.edge;
                evaluation.active_fault_snapshot = rule_evaluation.snapshot;
                evaluation.aggregated_state = RuleState::FAULT;
                continue;
            }

            if (evaluation.aggregated_state != RuleState::FAULT &&
                rule_evaluation.state == RuleState::WARNING) {
                evaluation.aggregated_state = RuleState::WARNING;
            }
        }

        return evaluation;
    }

    void clear_runtime_state() { last_fault_publish_tick = 0; }

    uint64_t get_last_fault_publish_tick() const { return last_fault_publish_tick; }

    void set_last_fault_publish_tick(uint64_t tick) { last_fault_publish_tick = tick; }

private:
    const char* name{nullptr};
    SampleSource<T> source;
    array<optional<RuleModel<T>>, Config::max_rules_per_protection> rules{};
    size_t rule_count{0};
    uint64_t last_fault_publish_tick{0};
};

} // namespace Protections
