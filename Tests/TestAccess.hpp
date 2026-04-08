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

struct ProtectionEngine {
    static void clear() {
        for (size_t protection_index = 0; protection_index < ::ProtectionEngine::protection_count;
             ++protection_index) {
            if (::ProtectionEngine::protections[protection_index].has_value()) {
                visit(
                    [](auto& protection) { protection.clear_runtime_state(); },
                    *::ProtectionEngine::protections[protection_index]
                );
                ::ProtectionEngine::protections[protection_index].reset();
            }
        }
        ::ProtectionEngine::protection_count = 0;
        ::ProtectionEngine::registration_locked = false;
    }
};

} // namespace ST_LIB::TestAccess
