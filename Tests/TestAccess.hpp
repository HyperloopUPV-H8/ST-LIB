#pragma once

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionEngine.hpp"

namespace ST_LIB::TestAccess {

struct DiagnosticsHub {
    static void clear() {
        for (size_t sink_index = 0; sink_index < Diagnostics::Hub::sink_count; ++sink_index) {
            Diagnostics::Hub::sink_storage[sink_index].reset();
            Diagnostics::Hub::sinks[sink_index] = nullptr;
        }
        Diagnostics::Hub::sink_count = 0;
        Diagnostics::Hub::history_count = 0;
        Diagnostics::Hub::history_next_index = 0;
        Diagnostics::Hub::pending_count = 0;
        Diagnostics::Runtime::defaults_installed = false;
    }

    static size_t history_size() { return Diagnostics::Hub::history_count; }

    static size_t pending_size() { return Diagnostics::Hub::pending_count; }
};

struct FaultController {
    static void clear() {
        ::FaultController::reset_runtime_storage();
        ::FaultController::global_machine = nullptr;
        ::FaultController::on_fault_enter = nullptr;
        ::FaultController::latched_cause = {};
        ::FaultController::has_latched_cause = false;
        ::FaultController::faulted = false;
        ::FaultController::runtime_started = false;
#ifdef STLIB_ETH
        ::FaultController::propagation_targets = {};
        ::FaultController::propagation_target_count = 0;
#endif
    }

    static void request_fault(const ::FaultCause& cause) {
        ::FaultController::request_fault(cause);
    }
};

} // namespace ST_LIB::TestAccess
