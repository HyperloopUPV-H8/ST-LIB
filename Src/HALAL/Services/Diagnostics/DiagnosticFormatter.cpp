#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

namespace Diagnostics {

namespace {

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

void append_formatted(char* buffer, size_t buffer_size, const char* format, ...) {
    if (buffer_size == 0) {
        return;
    }

    const size_t offset = bounded_strnlen(buffer, buffer_size - 1);
    if (offset >= buffer_size - 1) {
        buffer[buffer_size - 1] = '\0';
        return;
    }

    va_list arguments;
    va_start(arguments, format);
    vsnprintf(buffer + offset, buffer_size - offset, format, arguments);
    va_end(arguments);
}

const char* rule_kind_label(Protections::RuleKind kind) {
    switch (kind) {
    case Protections::RuleKind::BELOW:
        return "below";
    case Protections::RuleKind::ABOVE:
        return "above";
    case Protections::RuleKind::RANGE:
        return "range";
    case Protections::RuleKind::EQUALS:
        return "equals";
    case Protections::RuleKind::NOT_EQUALS:
        return "not_equals";
    case Protections::RuleKind::TIME_ACCUMULATION:
        return "time_accumulation";
    }
    std::unreachable();
}

const char* rule_state_label(Protections::RuleState state) {
    switch (state) {
    case Protections::RuleState::FAULT:
        return "fault";
    case Protections::RuleState::WARNING:
        return "warning";
    case Protections::RuleState::NORMAL:
        return "normal";
    }
    std::unreachable();
}

const char* rule_edge_label(Protections::RuleEdge edge) {
    switch (edge) {
    case Protections::RuleEdge::FAULT_RAISED:
        return "fault_raised";
    case Protections::RuleEdge::WARNING_RAISED:
        return "warning_raised";
    case Protections::RuleEdge::RECOVERED:
        return "recovered";
    case Protections::RuleEdge::NONE:
        return "none";
    }
    std::unreachable();
}

void format_numeric_value(
    Protections::SampleEncoding encoding,
    Protections::NumericValue value,
    char* buffer,
    size_t buffer_size
) {
    if (buffer_size == 0) {
        return;
    }

    switch (encoding) {
    case Protections::SampleEncoding::BOOL:
        snprintf(buffer, buffer_size, "%s", value.bool_value ? "true" : "false");
        return;
    case Protections::SampleEncoding::SIGNED:
        snprintf(buffer, buffer_size, "%lld", static_cast<long long>(value.signed_value));
        return;
    case Protections::SampleEncoding::UNSIGNED:
        snprintf(
            buffer,
            buffer_size,
            "%llu",
            static_cast<unsigned long long>(value.unsigned_value)
        );
        return;
    case Protections::SampleEncoding::FLOAT32:
        snprintf(buffer, buffer_size, "%.6f", static_cast<double>(value.float32_value));
        return;
    case Protections::SampleEncoding::FLOAT64:
        snprintf(buffer, buffer_size, "%.6f", value.float64_value);
        return;
    }

    std::unreachable();
}

void append_timestamp_suffix(const Timestamp& timestamp, char* buffer, size_t buffer_size) {
    if (timestamp.has_rtc) {
        append_formatted(
            buffer,
            buffer_size,
            " | Timestamp: %04u-%02u-%02u %02u:%02u:%02u.%05u",
            static_cast<unsigned>(timestamp.year),
            static_cast<unsigned>(timestamp.month),
            static_cast<unsigned>(timestamp.day),
            static_cast<unsigned>(timestamp.hour),
            static_cast<unsigned>(timestamp.minute),
            static_cast<unsigned>(timestamp.second),
            static_cast<unsigned>(timestamp.counter)
        );
        return;
    }

#ifdef HAL_TIM_MODULE_ENABLED
    const uint64_t total_seconds = timestamp.uptime_us / 1'000'000ULL;
    const uint64_t days = total_seconds / 86'400ULL;
    const unsigned hours = static_cast<unsigned>((total_seconds / 3'600ULL) % 24ULL);
    const unsigned minutes = static_cast<unsigned>((total_seconds / 60ULL) % 60ULL);
    const unsigned seconds = static_cast<unsigned>(total_seconds % 60ULL);
    const unsigned micros = static_cast<unsigned>(timestamp.uptime_us % 1'000'000ULL);

    if (days > 0) {
        append_formatted(
            buffer,
            buffer_size,
            " | Uptime: %llud %02u:%02u:%02u.%06u",
            static_cast<unsigned long long>(days),
            hours,
            minutes,
            seconds,
            micros
        );
    } else {
        append_formatted(
            buffer,
            buffer_size,
            " | Uptime: %02u:%02u:%02u.%06u",
            hours,
            minutes,
            seconds,
            micros
        );
    }
#else
    (void)timestamp;
    (void)buffer;
    (void)buffer_size;
#endif
}

void format_runtime_record(
    const DiagnosticRecord& record,
    char* buffer,
    size_t buffer_size
) {
    const RuntimeDiagnosticPayload& runtime = record.payload.runtime;
    append_formatted(
        buffer,
        buffer_size,
        "%s | Line: %lu Function: '%s' File: %s",
        runtime.message,
        static_cast<unsigned long>(runtime.line),
        runtime.function_name,
        runtime.file_name
    );
    append_timestamp_suffix(record.timestamp, buffer, buffer_size);
    if (runtime.truncated) {
        append_formatted(buffer, buffer_size, " | Message truncated");
    }
}

void format_protection_record(
    const DiagnosticRecord& record,
    char* buffer,
    size_t buffer_size
) {
    const ProtectionDiagnosticPayload& protection = record.payload.protection;
    char value_buffer[32]{};
    char threshold_a_buffer[32]{};
    char threshold_b_buffer[32]{};

    format_numeric_value(
        protection.sample_encoding,
        protection.observed_value,
        value_buffer,
        sizeof(value_buffer)
    );
    format_numeric_value(
        protection.sample_encoding,
        protection.threshold_a,
        threshold_a_buffer,
        sizeof(threshold_a_buffer)
    );
    if (protection.has_threshold_b) {
        format_numeric_value(
            protection.sample_encoding,
            protection.threshold_b,
            threshold_b_buffer,
            sizeof(threshold_b_buffer)
        );
    }

    append_formatted(
        buffer,
        buffer_size,
        "Protection %s [%s] %s | Rule: %s | Value: %s | ThresholdA: %s",
        rule_state_label(protection.state),
        rule_edge_label(protection.edge),
        record.origin,
        rule_kind_label(protection.rule_kind),
        value_buffer,
        threshold_a_buffer
    );
    if (protection.has_threshold_b) {
        append_formatted(buffer, buffer_size, " | ThresholdB: %s", threshold_b_buffer);
    }
    if (protection.rule_kind == Protections::RuleKind::TIME_ACCUMULATION) {
        append_formatted(
            buffer,
            buffer_size,
            " | Window: %.3fs | Rate: %.3fHz",
            static_cast<double>(protection.time_window_s),
            static_cast<double>(protection.sample_rate_hz)
        );
    }
    append_timestamp_suffix(record.timestamp, buffer, buffer_size);
}

} // namespace

void DiagnosticFormatter::describe(
    const DiagnosticRecord& record,
    char* buffer,
    size_t buffer_size
) {
    if (buffer_size == 0) {
        return;
    }

    buffer[0] = '\0';
    switch (record.category) {
    case Category::RUNTIME_PANIC:
    case Category::RUNTIME_FAULT:
    case Category::RUNTIME_WARNING:
    case Category::RUNTIME_INFO:
        format_runtime_record(record, buffer, buffer_size);
        return;
    case Category::PROTECTION_EVENT:
        format_protection_record(record, buffer, buffer_size);
        return;
    }

    std::unreachable();
}

} // namespace Diagnostics
