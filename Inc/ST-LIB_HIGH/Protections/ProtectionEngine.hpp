#pragma once

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"

namespace ST_LIB::TestAccess {
struct ProtectionEngine;
}

namespace Protections {

template <ProtectionSample T> class ProtectionHandle {
public:
    explicit ProtectionHandle(Protection<T>* protection = nullptr) : protection(protection) {}

    expected<void, ProtectionError>
    add_rule(expected<RuleDefinition<T>, RuleConfigError> definition) {
        if (protection == nullptr) {
            return unexpected(ProtectionError::INVALID_HANDLE);
        }
        return protection->add_rule(std::move(definition));
    }

    expected<void, ProtectionError> add_rule(const RuleDefinition<T>& definition) {
        if (protection == nullptr) {
            return unexpected(ProtectionError::INVALID_HANDLE);
        }
        return protection->add_rule(definition);
    }

private:
    Protection<T>* protection{nullptr};
};

using ProtectionVariant = variant<
    Protection<bool>,
    Protection<int8_t>,
    Protection<uint8_t>,
    Protection<int16_t>,
    Protection<uint16_t>,
    Protection<int32_t>,
    Protection<uint32_t>,
    Protection<int64_t>,
    Protection<uint64_t>,
    Protection<float>,
    Protection<double>>;

} // namespace Protections

class ProtectionEngine {
public:
    template <Protections::ReadableSampleSource Source>
    static expected<
        Protections::ProtectionHandle<typename std::remove_cvref_t<Source>::value_type>,
        Protections::ProtectionError>
    create_protection(const char* name, Source source) {
        using SampleType = typename std::remove_cvref_t<Source>::value_type;

        if (registration_locked) {
            return unexpected(Protections::ProtectionError::REGISTRATION_LOCKED);
        }
        if (protection_count >= Protections::Config::max_protections) {
            return unexpected(Protections::ProtectionError::PROTECTION_CAPACITY_EXCEEDED);
        }

        auto& slot = protections[protection_count++];
        slot = Protections::ProtectionVariant(
            std::in_place_type<Protections::Protection<SampleType>>,
            name,
            source
        );
        auto* protection = std::get_if<Protections::Protection<SampleType>>(&slot.value());
        return Protections::ProtectionHandle<SampleType>{protection};
    }

    static void initialize();
    static void evaluate();

private:
    friend struct ST_LIB::TestAccess::ProtectionEngine;

    template <typename Protection>
    static void request_fault_if_due(
        Protection& protection,
        const Protections::ProtectionEvaluation& evaluation
    );

    template <typename Protection>
    static void publish_edge_events(
        Protection& protection,
        const Protections::ProtectionEvaluation& evaluation
    );

    static array<optional<Protections::ProtectionVariant>, Protections::Config::max_protections>
        protections;
    static size_t protection_count;
    static bool registration_locked;
};
