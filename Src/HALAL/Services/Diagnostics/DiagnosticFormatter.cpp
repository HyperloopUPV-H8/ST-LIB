#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

namespace Diagnostics {

namespace {

struct DiagnosticStringBuilder {
    char* buffer;
    size_t buffer_size;
    size_t offset{0};

    DiagnosticStringBuilder(char* target, size_t target_size)
        : buffer(target), buffer_size(target_size) {
        if (buffer_size > 0) {
            buffer[0] = '\0';
        }
    }

    void append(const char* format, ...) {
        if (buffer_size == 0 || offset >= buffer_size - 1) {
            return;
        }

        va_list arguments;
        va_start(arguments, format);
        const int32_t written =
            vsnprintf(buffer + offset, buffer_size - offset, format, arguments);
        va_end(arguments);

        if (written < 0) {
            return;
        }

        const size_t remaining = buffer_size - offset;
        if (static_cast<size_t>(written) >= remaining) {
            offset = buffer_size - 1;
            return;
        }

        offset += static_cast<size_t>(written);
    }
};

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
        {
            const char* text = value.bool_value ? "true" : "false";
            const size_t length = strnlen(text, buffer_size - 1);
            memcpy(buffer, text, length);
            buffer[length] = '\0';
        }
        return;
    case Protections::SampleEncoding::SIGNED:
        if (snprintf(buffer, buffer_size, "%lld", static_cast<long long>(value.signed_value)) <
            0) {
            buffer[0] = '\0';
        }
        return;
    case Protections::SampleEncoding::UNSIGNED:
        if (snprintf(
                buffer,
                buffer_size,
                "%llu",
                static_cast<unsigned long long>(value.unsigned_value)
            ) < 0) {
            buffer[0] = '\0';
        }
        return;
    case Protections::SampleEncoding::FLOAT32:
        if (snprintf(buffer, buffer_size, "%.6f", static_cast<double>(value.float32_value)) <
            0) {
            buffer[0] = '\0';
        }
        return;
    case Protections::SampleEncoding::FLOAT64:
        if (snprintf(buffer, buffer_size, "%.6f", value.float64_value) < 0) {
            buffer[0] = '\0';
        }
        return;
    }

    std::unreachable();
}

void append_timestamp_suffix(const Timestamp& timestamp, DiagnosticStringBuilder& builder) {
    if (timestamp.has_rtc) {
        builder.append(
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
        builder.append(
            " | Uptime: %llud %02u:%02u:%02u.%06u",
            static_cast<unsigned long long>(days),
            hours,
            minutes,
            seconds,
            micros
        );
    } else {
        builder.append(" | Uptime: %02u:%02u:%02u.%06u", hours, minutes, seconds, micros);
    }
#else
    (void)timestamp;
    (void)builder;
#endif
}

void format_runtime_record(const DiagnosticRecord& record, DiagnosticStringBuilder& builder) {
    const RuntimeDiagnosticPayload& runtime = record.payload.runtime;
    builder.append(
        "%s | Line: %lu Function: '%s' File: %s",
        runtime.message,
        static_cast<unsigned long>(runtime.line),
        runtime.function_name,
        runtime.file_name
    );
    append_timestamp_suffix(record.timestamp, builder);
    if (runtime.truncated) {
        builder.append(" | Message truncated");
    }
}

void format_protection_record(const DiagnosticRecord& record, DiagnosticStringBuilder& builder) {
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

    builder.append(
        "Protection %s [%s] %s | Rule: %s | Value: %s | ThresholdA: %s",
        rule_state_label(protection.state),
        rule_edge_label(protection.edge),
        record.origin,
        rule_kind_label(protection.rule_kind),
        value_buffer,
        threshold_a_buffer
    );
    if (protection.has_threshold_b) {
        builder.append(" | ThresholdB: %s", threshold_b_buffer);
    }
    if (protection.rule_kind == Protections::RuleKind::TIME_ACCUMULATION) {
        builder.append(
            " | Window: %.3fs | Active: %.3fs",
            static_cast<double>(protection.time_window_s),
            static_cast<double>(protection.active_time_s)
        );
    }
    append_timestamp_suffix(record.timestamp, builder);
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

    DiagnosticStringBuilder builder{buffer, buffer_size};
    switch (record.category) {
    case Category::RUNTIME_PANIC:
    case Category::RUNTIME_FAULT:
    case Category::RUNTIME_WARNING:
    case Category::RUNTIME_INFO:
        format_runtime_record(record, builder);
        return;
    case Category::PROTECTION_EVENT:
        format_protection_record(record, builder);
        return;
    }

    std::unreachable();
}

} // namespace Diagnostics
