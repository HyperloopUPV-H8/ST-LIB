#pragma once

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"

namespace Protections {

namespace detail {

template <typename Source>
concept SampleSourceLike = requires(const std::remove_cvref_t<Source>& source) {
    typename std::remove_cvref_t<Source>::value_type;
    source.read();
};

template <typename Source> consteval auto sample_type_tag() {
    using source_type = std::remove_cvref_t<Source>;
    if constexpr (requires { typename source_type::value_type; }) {
        return std::type_identity<typename source_type::value_type>{};
    } else {
        return std::type_identity<source_type>{};
    }
}

template <typename Source>
using sample_type_from_source_t = typename decltype(sample_type_tag<Source>())::type;

template <typename Source> constexpr auto to_sample_source(Source& source) {
    if constexpr (SampleSourceLike<Source>) {
        return source;
    } else {
        using SampleType = sample_type_from_source_t<Source>;
        return SampleSource<SampleType>{source};
    }
}

} // namespace detail

template <ProtectionSample T, std::size_t N> struct BakedRules {
    using sample_type = T;
    static constexpr std::size_t rule_count = N;

    std::array<RuleDefinition<T>, N> definitions{};
};

template <ProtectionSample T, typename... RuleDefs>
    requires((std::same_as<std::remove_cvref_t<RuleDefs>, RuleDefinition<T>> && ...))
constexpr auto bake_rules(RuleDefs... definitions) {
    return BakedRules<T, sizeof...(RuleDefs)>{
        std::array<RuleDefinition<T>, sizeof...(RuleDefs)>{definitions...}
    };
}

template <std::size_t N> struct FixedString {
    char value[N]{};

    constexpr FixedString(const char (&str)[N]) {
        for (std::size_t index = 0; index < N; ++index) {
            value[index] = str[index];
        }
    }

    constexpr const char* c_str() const { return value; }
    constexpr std::size_t size() const { return N - 1; }
};

template <std::size_t N> FixedString(const char (&)[N]) -> FixedString<N>;

template <FixedString Name, auto& Source> struct ProtectionSpec {
    using source_type = std::remove_cvref_t<decltype(Source)>;
    using sample_type = detail::sample_type_from_source_t<source_type>;

    static constexpr auto name = Name;
    static constexpr auto& source = Source;
};

template <FixedString Name, auto& Source, auto& Rules> struct ProtectionSpecWithRules {
    using source_type = std::remove_cvref_t<decltype(Source)>;
    using sample_type = detail::sample_type_from_source_t<source_type>;
    using baked_rules_type = std::remove_cvref_t<decltype(Rules)>;

    static constexpr auto name = Name;
    static constexpr auto& source = Source;
    static constexpr auto& baked_rules = Rules;
};

template <FixedString Name, auto& Source>
using ProtectionDeclaration = ProtectionSpec<Name, Source>;

template <FixedString Name, auto& Source, auto& Rules>
using ProtectionDeclarationWithRules = ProtectionSpecWithRules<Name, Source, Rules>;

template <typename... ProtectionSpecs> class ProtectionEngine {
public:
    static constexpr std::size_t protection_count = sizeof...(ProtectionSpecs);

    template <typename Spec>
    static constexpr bool has_baked_rules = requires { typename Spec::baked_rules_type; };

    template <typename Spec> struct StorageForSpec;

    template <typename Spec>
        requires has_baked_rules<Spec>
    struct StorageForSpec<Spec> {
        using type = Protection<typename Spec::sample_type, Spec::baked_rules_type::rule_count>;
    };

    template <typename Spec>
        requires(!has_baked_rules<Spec>)
    struct StorageForSpec<Spec> {
        using type = Protection<typename Spec::sample_type, 0>;
    };

    template <typename Spec> using storage_for_spec_t = typename StorageForSpec<Spec>::type;

    using Storage = std::tuple<storage_for_spec_t<ProtectionSpecs>...>;

    static void initialize() {
#if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
        Global_RTC::ensure_started();
#endif
        initialize_impl(std::make_index_sequence<protection_count>{});
    }

    static void evaluate() { evaluate_impl(std::make_index_sequence<protection_count>{}); }

    template <std::size_t Index> static auto& protection_at() {
        return std::get<Index>(protections);
    }

    template <typename ProtectionSpec> static auto& protection() {
        return protection_at<spec_index<ProtectionSpec>()>();
    }

    static void reset() { reset_impl(std::make_index_sequence<protection_count>{}); }

private:
    template <typename ProtectionSpec> static constexpr auto make_protection() {
        using SampleType = typename ProtectionSpec::sample_type;

        if constexpr (has_baked_rules<ProtectionSpec>) {
            using RulesType = typename ProtectionSpec::baked_rules_type;
            static_assert(
                std::same_as<typename RulesType::sample_type, SampleType>,
                "Baked rules sample type must match protection sample type"
            );

            return Protection<SampleType, RulesType::rule_count>{
                ProtectionSpec::name.c_str(),
                detail::to_sample_source(ProtectionSpec::source),
                ProtectionSpec::baked_rules.definitions
            };
        } else {
            constexpr std::array<RuleDefinition<SampleType>, 0> empty_rules{};
            return Protection<SampleType, 0>{
                ProtectionSpec::name.c_str(),
                detail::to_sample_source(ProtectionSpec::source),
                empty_rules
            };
        }
    }

    template <std::size_t... Indices> static void initialize_impl(std::index_sequence<Indices...>) {
        (std::get<Indices>(protections).initialize(), ...);
    }

    template <std::size_t... Indices> static void evaluate_impl(std::index_sequence<Indices...>) {
        (evaluate_one<Indices>(), ...);
    }

    template <std::size_t... Indices> static void reset_impl(std::index_sequence<Indices...>) {
        (std::get<Indices>(protections).clear_runtime_state(), ...);
    }

    template <std::size_t Index> static void evaluate_one() {
        auto& protection_ref = std::get<Index>(protections);
        const Protections::ProtectionEvaluation evaluation = protection_ref.evaluate();

        publish_edge_events(protection_ref, evaluation);
        if (evaluation.has_active_fault) {
            request_fault_if_due(protection_ref, evaluation);
        }
    }

    template <typename ProtectionType>
    static void publish_edge_events(
        ProtectionType& protection_ref,
        const Protections::ProtectionEvaluation& evaluation
    ) {
        for (std::size_t event_index = 0; event_index < evaluation.event_count; ++event_index) {
            const auto& event = evaluation.events[event_index];
            if (event.state == Protections::RuleState::FAULT) {
                continue;
            }

            Diagnostics::Hub::publish_protection_event(
                protection_ref.get_name(),
                event.state,
                event.edge,
                event.snapshot
            );
        }
    }

    template <typename ProtectionType>
    static void request_fault_if_due(
        ProtectionType& protection_ref,
        const Protections::ProtectionEvaluation& evaluation
    ) {
        const uint64_t tick = Scheduler::get_global_tick();
        const uint64_t last_publish_tick = protection_ref.get_last_fault_publish_tick();

        if (last_publish_tick != 0 &&
            tick < last_publish_tick + Protections::Config::notify_delay_in_microseconds) {
            return;
        }

        FaultController::request_fault(FaultCause::protection(
            protection_ref.get_name(),
            evaluation.active_fault_edge,
            evaluation.active_fault_snapshot
        ));
        protection_ref.set_last_fault_publish_tick(tick);
    }

    template <typename ProtectionSpec, std::size_t Index = 0>
    static consteval std::size_t spec_index() {
        if constexpr (Index >= protection_count) {
            static_assert([] { return false; }(), "Protection spec not found");
            return 0;
        } else if constexpr (std::is_same_v<
                                 ProtectionSpec,
                                 std::tuple_element_t<Index, std::tuple<ProtectionSpecs...>>>) {
            return Index;
        } else {
            return spec_index<ProtectionSpec, Index + 1>();
        }
    }

    inline static Storage protections{make_protection<ProtectionSpecs>()...};
};

} // namespace Protections

using ProtectionEngine = Protections::ProtectionEngine<>;
