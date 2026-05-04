#pragma once

#include <cmath>

#include "C++Utilities/CppUtils.hpp"
#include "HALAL/Services/Time/Scheduler.hpp"
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

template <typename T> constexpr bool is_below(T a, T b) { return a < b; }
template <typename T> constexpr bool is_above(T a, T b) { return a > b; }
template <typename T> constexpr bool is_equal_to(T a, T b) { return a == b; }
template <typename T> constexpr bool is_not_equal_to(T a, T b) { return a != b; }

} // namespace detail

// ── Branchless edge table  ──
inline constexpr RuleEdge edge_lut[3][3] = {
    {RuleEdge::NONE,           RuleEdge::WARNING_RAISED,  RuleEdge::FAULT_RAISED},
    {RuleEdge::RECOVERED,      RuleEdge::NONE,            RuleEdge::FAULT_RAISED},
    {RuleEdge::RECOVERED,      RuleEdge::WARNING_RAISED,  RuleEdge::NONE},
};

// ── Time-accumulation runtime state ──
struct TimeAccumState {
    bool     has_tick{false};
    uint64_t last_tick{0};
    uint64_t fault_time{0};
    uint64_t warn_time{0};
    float    active_time_s{0.0f};
};

// ── Function pointer type for rule evaluation ──
using EvalFn = RuleState (*)(const void* config, TimeAccumState* ta, const void* sample, uint64_t now_us);

// ── Typed evaluators (one per (T, RuleKind), shared across protections) ──
template <typename T>
inline RuleState eval_below(const void* cfg, TimeAccumState*, const void* sample, uint64_t) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const BelowRuleConfig<T>*>(cfg);
    if (detail::is_below(s, c.fault_threshold)) return RuleState::FAULT;
    if (c.warning_threshold.has_value() &&
        detail::is_below(s, c.warning_threshold.value())) return RuleState::WARNING;
    return RuleState::NORMAL;
}

template <typename T>
inline RuleState eval_above(const void* cfg, TimeAccumState*, const void* sample, uint64_t) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const AboveRuleConfig<T>*>(cfg);
    if (detail::is_above(s, c.fault_threshold)) return RuleState::FAULT;
    if (c.warning_threshold.has_value() &&
        detail::is_above(s, c.warning_threshold.value())) return RuleState::WARNING;
    return RuleState::NORMAL;
}

template <typename T>
inline RuleState eval_range(const void* cfg, TimeAccumState*, const void* sample, uint64_t) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const RangeRuleConfig<T>*>(cfg);
    if (detail::is_below(s, c.low_fault) || detail::is_above(s, c.high_fault))
        return RuleState::FAULT;
    if (c.low_warning.has_value() && c.high_warning.has_value() &&
        (detail::is_below(s, c.low_warning.value()) ||
         detail::is_above(s, c.high_warning.value())))
        return RuleState::WARNING;
    return RuleState::NORMAL;
}

template <typename T>
inline RuleState eval_equals(const void* cfg, TimeAccumState*, const void* sample, uint64_t) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const EqualsRuleConfig<T>*>(cfg);
    return detail::is_equal_to(s, c.expected) ? RuleState::FAULT : RuleState::NORMAL;
}

template <typename T>
inline RuleState eval_not_equals(const void* cfg, TimeAccumState*, const void* sample, uint64_t) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const NotEqualsRuleConfig<T>*>(cfg);
    return detail::is_not_equal_to(s, c.expected) ? RuleState::FAULT : RuleState::NORMAL;
}

template <FloatingSample T>
inline RuleState eval_time_accum(const void* cfg, TimeAccumState* ta, const void* sample, uint64_t now_us) {
    const auto& s = *static_cast<const T*>(sample);
    const auto& c = *static_cast<const TimeAccumulationRuleConfig<T>*>(cfg);

    const uint64_t elapsed = ta->has_tick ? (now_us - ta->last_tick) : 0ULL;
    ta->has_tick = true;
    ta->last_tick = now_us;

    const T mag = detail::absolute_value(s);

    if (detail::is_above(mag, c.fault_threshold)) ta->fault_time += elapsed;
    else                                          ta->fault_time = 0;

    if (c.warning_threshold.has_value() && detail::is_above(mag, c.warning_threshold.value()))
        ta->warn_time += elapsed;
    else
        ta->warn_time = 0;

    if (ta->fault_time >= c.window_us) {
        ta->active_time_s = static_cast<float>(ta->fault_time) / 1'000'000.0f;
        return RuleState::FAULT;
    }
    if (c.warning_threshold.has_value() && ta->warn_time >= c.window_us) {
        ta->active_time_s = static_cast<float>(ta->warn_time) / 1'000'000.0f;
        return RuleState::WARNING;
    }
    ta->active_time_s = static_cast<float>(
        c.warning_threshold.has_value() ? ta->warn_time : ta->fault_time
    ) / 1'000'000.0f;
    return RuleState::NORMAL;
}

// ── Compile-time dispatch helpers ──
template <typename T, RuleKind K> EvalFn get_eval_fn() {
    if constexpr (K == RuleKind::BELOW && ComparableSample<T>)             return &eval_below<T>;
    if constexpr (K == RuleKind::ABOVE && ComparableSample<T>)             return &eval_above<T>;
    if constexpr (K == RuleKind::RANGE && ComparableSample<T>)             return &eval_range<T>;
    if constexpr (K == RuleKind::EQUALS && EqualityComparableSample<T>)    return &eval_equals<T>;
    if constexpr (K == RuleKind::NOT_EQUALS && EqualityComparableSample<T>) return &eval_not_equals<T>;
    if constexpr (K == RuleKind::TIME_ACCUMULATION && FloatingSample<T>)   return &eval_time_accum<T>;
    return nullptr;
}

template <ProtectionSample T>
RuleKind rule_kind_of(const RuleDefinition<T>& def) {
    RuleKind k = RuleKind::BELOW;
    visit([&k](const auto& c) {
        using C = std::remove_cvref_t<decltype(c)>;
        if      constexpr (std::same_as<C, BelowRuleConfig<T>>)               k = RuleKind::BELOW;
        else if constexpr (std::same_as<C, AboveRuleConfig<T>>)               k = RuleKind::ABOVE;
        else if constexpr (std::same_as<C, RangeRuleConfig<T>>)               k = RuleKind::RANGE;
        else if constexpr (std::same_as<C, EqualsRuleConfig<T>>)              k = RuleKind::EQUALS;
        else if constexpr (std::same_as<C, NotEqualsRuleConfig<T>>)           k = RuleKind::NOT_EQUALS;
        else if constexpr (std::same_as<C, TimeAccumulationRuleConfig<T>>)    k = RuleKind::TIME_ACCUMULATION;
    }, def);
    return k;
}

template <ProtectionSample T>
const void* rule_cfg_ptr(const RuleDefinition<T>& def) {
    const void* p = nullptr;
    visit([&p](const auto& c) { p = &c; }, def);
    return p;
}

template <ProtectionSample T>
EvalFn evaluator_for(const RuleDefinition<T>& def) {
    const RuleKind k = rule_kind_of(def);
    switch (k) {
        case RuleKind::BELOW:             return get_eval_fn<T, RuleKind::BELOW>();
        case RuleKind::ABOVE:             return get_eval_fn<T, RuleKind::ABOVE>();
        case RuleKind::RANGE:             return get_eval_fn<T, RuleKind::RANGE>();
        case RuleKind::EQUALS:            return get_eval_fn<T, RuleKind::EQUALS>();
        case RuleKind::NOT_EQUALS:        return get_eval_fn<T, RuleKind::NOT_EQUALS>();
        case RuleKind::TIME_ACCUMULATION: return get_eval_fn<T, RuleKind::TIME_ACCUMULATION>();
    }
    return nullptr;
}

// ────────────────────────────────────────────────────────────────
// Ono std::variant
// ────────────────────────────────────────────────────────────────
#ifdef STLIB_OPTIMIZED_COMPARISON_TEST
namespace Optimized {
#endif

template <ProtectionSample T, std::size_t RuleCount> class Protection {
public:
    Protection(const char* name, SampleSource<T> source,
               const std::array<RuleDefinition<T>, RuleCount>& definitions)
        : name(name), source(source)
    {
        for (std::size_t i = 0; i < RuleCount; ++i) {
            rule_kinds[i]   = rule_kind_of(definitions[i]);
            eval_fns[i]     = evaluator_for<T>(definitions[i]);
            config_ptrs[i]  = rule_cfg_ptr(definitions[i]);
            prev_state[i]   = static_cast<uint8_t>(RuleState::NORMAL);
        }
    }

    const char* get_name() const { return name; }
    void initialize() {}

    ProtectionEvaluation evaluate(uint64_t now_us) {
        ProtectionEvaluation eval{};
        const T sample = source.read();

        for (std::size_t i = 0; i < RuleCount; ++i) {
            const RuleState state = eval_fns[i](config_ptrs[i], &ta_state[i], &sample, now_us);
            const RuleEdge edge = edge_lut[prev_state[i]][static_cast<uint8_t>(state)];
            prev_state[i] = static_cast<uint8_t>(state);

            if (edge != RuleEdge::NONE && eval.event_count < eval.events.size()) {
                eval.events[eval.event_count++] = {state, edge, build_snapshot(i, sample)};
            }

            if (state == RuleState::FAULT && !eval.has_active_fault) {
                eval.has_active_fault = true;
                eval.active_fault_edge = edge;
                eval.active_fault_snapshot = build_snapshot(i, sample);
                eval.aggregated_state = RuleState::FAULT;
                continue;
            }
            if (eval.aggregated_state != RuleState::FAULT && state == RuleState::WARNING) {
                eval.aggregated_state = RuleState::WARNING;
            }
        }
        return eval;
    }

    ProtectionEvaluation evaluate() {
        return evaluate(Scheduler::get_global_tick());
    }

    void clear_runtime_state() {
        for (std::size_t i = 0; i < RuleCount; ++i) {
            prev_state[i] = static_cast<uint8_t>(RuleState::NORMAL);
            ta_state[i] = TimeAccumState{};
        }
        last_fault_publish_tick = 0;
    }

    uint64_t get_last_fault_publish_tick() const { return last_fault_publish_tick; }
    void set_last_fault_publish_tick(uint64_t t) { last_fault_publish_tick = t; }

private:
    RuleSnapshot build_snapshot(std::size_t i, T sample) const {
        RuleSnapshot snap{};
        const auto ki = rule_kinds[i];
        snap.kind = ki;
        snap.sample_encoding = sample_encoding_for<T>();
        snap.observed_value = to_numeric_value(sample);

        const void* cfg = config_ptrs[i];
        switch (ki) {
        case RuleKind::BELOW: {
            auto& c = *static_cast<const BelowRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.fault_threshold);
            if (c.warning_threshold.has_value()) snap.uses_warning_threshold = true;
            break;
        }
        case RuleKind::ABOVE: {
            auto& c = *static_cast<const AboveRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.fault_threshold);
            if (c.warning_threshold.has_value()) snap.uses_warning_threshold = true;
            break;
        }
        case RuleKind::RANGE: {
            auto& c = *static_cast<const RangeRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.low_fault);
            snap.threshold_b = to_numeric_value(c.high_fault);
            snap.has_threshold_b = true;
            break;
        }
        case RuleKind::EQUALS: {
            auto& c = *static_cast<const EqualsRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.expected);
            break;
        }
        case RuleKind::NOT_EQUALS: {
            auto& c = *static_cast<const NotEqualsRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.expected);
            break;
        }
        case RuleKind::TIME_ACCUMULATION: {
            auto& c = *static_cast<const TimeAccumulationRuleConfig<T>*>(cfg);
            snap.threshold_a = to_numeric_value(c.fault_threshold);
            if (c.warning_threshold.has_value()) snap.uses_warning_threshold = true;
            snap.time_window_s = c.time_window_s;
            snap.active_time_s = ta_state[i].active_time_s;
            break;
        }
        }
        return snap;
    }

    const char* name;
    SampleSource<T> source;

    RuleKind      rule_kinds[RuleCount]{};
    EvalFn        eval_fns[RuleCount]{};
    const void*   config_ptrs[RuleCount]{};
    uint8_t       prev_state[RuleCount]{};
    TimeAccumState ta_state[RuleCount]{};
    uint64_t      last_fault_publish_tick{0};
};

#ifdef STLIB_OPTIMIZED_COMPARISON_TEST
} // namespace Optimized
#endif

} // namespace Protections
