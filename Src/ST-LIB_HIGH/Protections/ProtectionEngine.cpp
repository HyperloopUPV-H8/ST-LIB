#include "ST-LIB_HIGH/Protections/ProtectionEngine.hpp"

#include "HALAL/Services/Time/RTC.hpp"
#include "HALAL/Services/Time/Scheduler.hpp"

array<optional<Protections::ProtectionVariant>, Protections::Config::max_protections>
    ProtectionEngine::protections = {};
size_t ProtectionEngine::protection_count = 0;
bool ProtectionEngine::registration_locked = false;

void ProtectionEngine::initialize() {
#if defined(HAL_RTC_MODULE_ENABLED) && !defined(SIM_ON)
    Global_RTC::ensure_started();
#endif

    registration_locked = true;
    for (size_t protection_index = 0; protection_index < protection_count; protection_index++) {
        if (protections[protection_index].has_value()) {
            visit([](auto& protection) { protection.initialize(); }, *protections[protection_index]);
        }
    }
}

void ProtectionEngine::link_state_machine(
    IStateMachine& general_state_machine,
    ProtectionEngine::state_id fault_id
) {
    FaultController::link_state_machine(general_state_machine, fault_id);
}

template <typename Protection>
void ProtectionEngine::publish_fault_if_due(
    Protection& protection,
    const Protections::ProtectionEvaluation& evaluation
) {
    if (!evaluation.has_active_fault) {
        return;
    }

    const uint64_t tick = Scheduler::get_global_tick();
    const uint64_t last_publish_tick = protection.get_last_fault_publish_tick();
    if (last_publish_tick != 0 &&
        tick < last_publish_tick + Protections::Config::notify_delay_in_microseconds) {
        return;
    }

    Diagnostics::Hub::publish_protection_event(
        protection.get_name(),
        Protections::RuleState::FAULT,
        evaluation.active_fault_edge,
        evaluation.active_fault_snapshot
    );
    protection.set_last_fault_publish_tick(tick);
}

template <typename Protection>
void ProtectionEngine::publish_edge_events(
    Protection& protection,
    const Protections::ProtectionEvaluation& evaluation
) {
    for (size_t event_index = 0; event_index < evaluation.event_count; event_index++) {
        const Protections::ProtectionEvent& event = evaluation.events[event_index];
        if (event.state == Protections::RuleState::FAULT) {
            continue;
        }
        Diagnostics::Hub::publish_protection_event(
            protection.get_name(),
            event.state,
            event.edge,
            event.snapshot
        );
    }
}

void ProtectionEngine::evaluate() {
    for (size_t protection_index = 0; protection_index < protection_count; protection_index++) {
        if (!protections[protection_index].has_value()) {
            continue;
        }

        visit(
            [](auto& protection) {
                const Protections::ProtectionEvaluation evaluation = protection.evaluate();
                publish_edge_events(protection, evaluation);

                if (evaluation.has_active_fault) {
                    publish_fault_if_due(protection, evaluation);
                    FaultController::enter_fault();
                }
            },
            *protections[protection_index]
        );
    }
}

void ProtectionEngine::clear_for_testing() {
    for (size_t protection_index = 0; protection_index < protection_count; protection_index++) {
        if (protections[protection_index].has_value()) {
            visit([](auto& protection) { protection.clear_runtime_state(); }, *protections[protection_index]);
            protections[protection_index].reset();
        }
    }
    protection_count = 0;
    registration_locked = false;
}
