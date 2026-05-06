#include "ST-LIB_HIGH/Protections/FaultController.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

namespace {

static_assert(FaultConfig::origin_capacity == Diagnostics::Config::origin_capacity);
static_assert(
    FaultConfig::runtime_message_capacity == Diagnostics::Config::runtime_message_capacity
);
static_assert(FaultConfig::function_capacity == Diagnostics::Config::function_capacity);
static_assert(FaultConfig::file_capacity == Diagnostics::Config::file_capacity);

template <size_t Capacity>
void copy_c_string(char (&dst)[Capacity], const char* src, bool* truncated = nullptr) {
    if (Capacity == 0) {
        if (truncated != nullptr) {
            *truncated = true;
        }
        return;
    }

    if (src == nullptr) {
        dst[0] = '\0';
        return;
    }

    const size_t length = strnlen(src, Capacity - 1);
    memcpy(dst, src, length);
    dst[length] = '\0';
    if (truncated != nullptr) {
        *truncated = *truncated || src[length] != '\0';
    }
}

namespace FaultDiagnosticMapper {

Diagnostics::DiagnosticRecord to_record(const FaultCause& cause) {
    switch (cause.kind) {
    case FaultCauseKind::PANIC:
        return Diagnostics::RecordFactory::runtime_panic(
            cause.runtime.message,
            cause.runtime.truncated,
            Diagnostics::RuntimeSourceMetadata{
                static_cast<int>(cause.runtime.line),
                cause.runtime.function_name,
                cause.runtime.file_name
            },
            Diagnostics::DiagnosticPriority::URGENT
        );
    case FaultCauseKind::RUNTIME_FAULT:
        return Diagnostics::RecordFactory::runtime_fault(
            cause.runtime.message,
            cause.runtime.truncated,
            Diagnostics::RuntimeSourceMetadata{
                static_cast<int>(cause.runtime.line),
                cause.runtime.function_name,
                cause.runtime.file_name
            },
            Diagnostics::DiagnosticPriority::URGENT
        );
    case FaultCauseKind::PROTECTION:
        return Diagnostics::RecordFactory::protection_event(
            cause.origin,
            Protections::RuleState::FAULT,
            cause.protection_event.edge,
            cause.protection_event.snapshot,
            Diagnostics::DiagnosticPriority::URGENT
        );
    }

    std::unreachable();
}

} // namespace FaultDiagnosticMapper

} // namespace

FaultController::RuntimeStorage FaultController::runtime_storage = {};
IStateMachine* FaultController::global_machine = nullptr;
Callback FaultController::on_fault_enter = nullptr;
FaultCause FaultController::latched_cause = {};
bool FaultController::has_latched_cause = false;
bool FaultController::faulted = false;
bool FaultController::runtime_started = false;

FaultCause FaultCause::panic(
    const char* message,
    bool truncated,
    int line,
    const char* func,
    const char* file
) {
    FaultCause cause{};
    cause.kind = FaultCauseKind::PANIC;
    copy_c_string(cause.origin, "runtime_panic");
    cause.runtime.line = static_cast<uint32_t>(line < 0 ? 0 : line);
    cause.runtime.truncated = truncated;
    copy_c_string(cause.runtime.message, message, &cause.runtime.truncated);
    copy_c_string(cause.runtime.function_name, func);
    copy_c_string(cause.runtime.file_name, file);
    return cause;
}

FaultCause FaultCause::runtime_fault(
    const char* message,
    bool truncated,
    int line,
    const char* func,
    const char* file
) {
    FaultCause cause{};
    cause.kind = FaultCauseKind::RUNTIME_FAULT;
    copy_c_string(cause.origin, "runtime_fault");
    cause.runtime.line = static_cast<uint32_t>(line < 0 ? 0 : line);
    cause.runtime.truncated = truncated;
    copy_c_string(cause.runtime.message, message, &cause.runtime.truncated);
    copy_c_string(cause.runtime.function_name, func);
    copy_c_string(cause.runtime.file_name, file);
    return cause;
}

FaultCause FaultCause::protection(
    const char* protection_name,
    Protections::RuleEdge edge,
    const Protections::RuleSnapshot& snapshot
) {
    FaultCause cause{};
    cause.kind = FaultCauseKind::PROTECTION;
    copy_c_string(cause.origin, protection_name);
    cause.protection_event.edge = edge;
    cause.protection_event.snapshot = snapshot;
    return cause;
}

void FaultController::reset_runtime_storage() {
    if (runtime_storage.machine != nullptr && runtime_storage.destroy != nullptr) {
        runtime_storage.destroy(runtime_storage.machine);
    }
    runtime_storage.machine = nullptr;
    runtime_storage.destroy = nullptr;
    runtime_storage.start = nullptr;
    runtime_storage.rebuild_as_fault = nullptr;
    global_machine = nullptr;
}

void FaultController::start() {
    if (global_machine == nullptr || runtime_storage.start == nullptr || runtime_started) {
        return;
    }

    runtime_storage.start(global_machine);
    runtime_started = true;

    if (faulted) {
        Diagnostics::Hub::flush_urgent();
    }
}

void FaultController::check_transitions() {
    if (global_machine == nullptr || !runtime_started) {
        return;
    }
    global_machine->check_transitions();
}

void FaultController::publish_fault_diagnostic(const FaultCause& cause) {
    Diagnostics::Hub::publish(FaultDiagnosticMapper::to_record(cause));
    Diagnostics::Hub::flush_urgent();
}

void FaultController::request_fault(const FaultCause& cause) {
    if (!faulted) {
        latched_cause = cause;
        has_latched_cause = true;
        faulted = true;

        if (global_machine != nullptr) {
            if (runtime_started) {
                global_machine->force_change_state(static_cast<size_t>(RuntimeState::FAULT));
            } else if (runtime_storage.rebuild_as_fault != nullptr) {
                runtime_storage.rebuild_as_fault();
            }
        }
    }

    publish_fault_diagnostic(cause);
}

bool FaultController::is_faulted() { return faulted; }

const FaultCause* FaultController::latched_fault_cause() {
    return has_latched_cause ? &latched_cause : nullptr;
}

void FaultController::on_fault_state_enter() {
    if (on_fault_enter != nullptr) {
        on_fault_enter();
    }
}
