#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

namespace Diagnostics {

array<Hub::SinkStorage, Config::max_sinks> Hub::sink_storage = {};
array<DiagnosticSink*, Config::max_sinks> Hub::sinks = {};
size_t Hub::sink_count = 0;
array<DiagnosticRecord, Config::history_capacity> Hub::history = {};
size_t Hub::history_count = 0;
size_t Hub::history_next_index = 0;
array<Hub::PendingRecord, Config::pending_capacity> Hub::pending_records = {};
size_t Hub::pending_count = 0;
bool Runtime::defaults_installed = false;

namespace {

constexpr const char* runtime_error_origin = "error_handler";
constexpr const char* runtime_warning_origin = "info_warning";

size_t bounded_strnlen(const char* src, size_t max_length) {
    if (src == nullptr) {
        return 0;
    }

    size_t length = 0;
    while (length < max_length && src[length] != '\0') {
        length++;
    }
    return length;
}

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

    const size_t length = bounded_strnlen(src, Capacity - 1);
    memcpy(dst, src, length);
    dst[length] = '\0';
    if (truncated != nullptr) {
        *truncated = *truncated || src[length] != '\0';
    }
}

} // namespace

void Hub::push_history(const DiagnosticRecord& record) {
    history[history_next_index] = record;
    history_next_index = (history_next_index + 1) % Config::history_capacity;
    if (history_count < Config::history_capacity) {
        history_count++;
    }
}

void Hub::push_pending(const DiagnosticRecord& record) {
    if (pending_count == Config::pending_capacity) {
        remove_pending(0);
    }

    pending_records[pending_count].record = record;
    pending_records[pending_count].delivered_mask = 0;
    pending_count++;
}

void Hub::remove_pending(size_t index) {
    if (index >= pending_count) {
        return;
    }

    for (size_t next = index + 1; next < pending_count; ++next) {
        pending_records[next - 1] = pending_records[next];
    }
    pending_count--;
}

void Hub::publish(DiagnosticRecord record) {
    push_history(record);

    if (sink_count == 0) {
        return;
    }

    push_pending(record);
}

void Hub::publish_runtime_error(
    const char* message,
    bool truncated,
    int line,
    const char* func,
    const char* file
) {
    DiagnosticRecord record{};
    record.severity = Severity::FAULT;
    record.category = Category::RUNTIME_ERROR;
    record.timestamp = DiagnosticTimestampProvider::capture();
    copy_c_string(record.origin, runtime_error_origin);
    record.payload.runtime.line = static_cast<uint32_t>(line < 0 ? 0 : line);
    record.payload.runtime.truncated = truncated;
    copy_c_string(record.payload.runtime.message, message, &record.payload.runtime.truncated);
    copy_c_string(record.payload.runtime.function_name, func);
    copy_c_string(record.payload.runtime.file_name, file);
    publish(record);
}

void Hub::publish_runtime_warning(
    const char* message,
    bool truncated,
    int line,
    const char* func,
    const char* file
) {
    DiagnosticRecord record{};
    record.severity = Severity::WARNING;
    record.category = Category::RUNTIME_WARNING;
    record.timestamp = DiagnosticTimestampProvider::capture();
    copy_c_string(record.origin, runtime_warning_origin);
    record.payload.runtime.line = static_cast<uint32_t>(line < 0 ? 0 : line);
    record.payload.runtime.truncated = truncated;
    copy_c_string(record.payload.runtime.message, message, &record.payload.runtime.truncated);
    copy_c_string(record.payload.runtime.function_name, func);
    copy_c_string(record.payload.runtime.file_name, file);
    publish(record);
}

void Hub::publish_protection_event(
    const char* protection_name,
    Protections::RuleState state,
    Protections::RuleEdge edge,
    const Protections::RuleSnapshot& snapshot
) {
    DiagnosticRecord record{};
    switch (state) {
    case Protections::RuleState::FAULT:
        record.severity = Severity::FAULT;
        break;
    case Protections::RuleState::WARNING:
        record.severity = Severity::WARNING;
        break;
    case Protections::RuleState::NORMAL:
        record.severity = Severity::INFO;
        break;
    }

    record.category = Category::PROTECTION_EVENT;
    record.timestamp = DiagnosticTimestampProvider::capture();
    copy_c_string(record.origin, protection_name);
    record.payload.protection.rule_kind = snapshot.kind;
    record.payload.protection.state = state;
    record.payload.protection.edge = edge;
    record.payload.protection.sample_encoding = snapshot.sample_encoding;
    record.payload.protection.observed_value = snapshot.observed_value;
    record.payload.protection.threshold_a = snapshot.threshold_a;
    record.payload.protection.threshold_b = snapshot.threshold_b;
    record.payload.protection.has_threshold_b = snapshot.has_threshold_b;
    record.payload.protection.uses_warning_threshold = snapshot.uses_warning_threshold;
    record.payload.protection.time_window_s = snapshot.time_window_s;
    record.payload.protection.sample_rate_hz = snapshot.sample_rate_hz;
    publish(record);
}

void Hub::flush() {
    const uint8_t target_mask =
        sink_count == 0 ? 0 : static_cast<uint8_t>((1u << sink_count) - 1u);

    for (size_t record_index = 0; record_index < pending_count;) {
        PendingRecord& pending_record = pending_records[record_index];

        for (size_t sink_index = 0; sink_index < sink_count; ++sink_index) {
            const uint8_t sink_mask = static_cast<uint8_t>(1u << sink_index);
            if ((pending_record.delivered_mask & sink_mask) != 0u) {
                continue;
            }
            if (sinks[sink_index] != nullptr && sinks[sink_index]->publish(pending_record.record)) {
                pending_record.delivered_mask |= sink_mask;
            }
        }

        if (pending_record.delivered_mask == target_mask) {
            remove_pending(record_index);
        } else {
            record_index++;
        }
    }
}

void Hub::clear_for_testing() {
    for (size_t sink_index = 0; sink_index < sink_count; ++sink_index) {
        sink_storage[sink_index].reset();
        sinks[sink_index] = nullptr;
    }
    sink_count = 0;
    history_count = 0;
    history_next_index = 0;
    pending_count = 0;
    Runtime::reset_for_testing();
}

size_t Hub::history_size_for_testing() { return history_count; }

size_t Hub::pending_size_for_testing() { return pending_count; }

void Runtime::reset_for_testing() { defaults_installed = false; }

} // namespace Diagnostics
