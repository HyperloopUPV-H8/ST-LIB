#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionTypes.hpp"

namespace Diagnostics {

namespace Config {
inline constexpr size_t max_sinks = 4;
inline constexpr size_t max_sink_storage = 1024;
inline constexpr size_t history_capacity = 16;
inline constexpr size_t pending_capacity = 16;
inline constexpr size_t origin_capacity = Protections::Config::max_name_length;
inline constexpr size_t runtime_message_capacity = 160;
inline constexpr size_t function_capacity = 64;
inline constexpr size_t file_capacity = 96;
inline constexpr size_t formatted_message_capacity = 320;
} // namespace Config

enum class Severity : uint8_t { INFO = 0, WARNING, FAULT };
enum class Category : uint8_t { RUNTIME_ERROR = 0, RUNTIME_WARNING, PROTECTION_EVENT };
enum class RegistrationError : uint8_t { CAPACITY_EXCEEDED = 0, STORAGE_TOO_SMALL };

struct Timestamp {
    bool has_rtc{false};
    uint16_t counter{0};
    uint8_t second{0};
    uint8_t minute{0};
    uint8_t hour{0};
    uint8_t day{0};
    uint8_t month{0};
    uint16_t year{0};
    uint64_t uptime_us{0};
};

struct RuntimeDiagnosticPayload {
    uint32_t line{0};
    bool truncated{false};
    char message[Config::runtime_message_capacity + 1]{};
    char function_name[Config::function_capacity + 1]{};
    char file_name[Config::file_capacity + 1]{};
};

struct ProtectionDiagnosticPayload {
    Protections::RuleKind rule_kind{Protections::RuleKind::BELOW};
    Protections::RuleState state{Protections::RuleState::NORMAL};
    Protections::RuleEdge edge{Protections::RuleEdge::NONE};
    Protections::SampleEncoding sample_encoding{Protections::SampleEncoding::SIGNED};
    Protections::NumericValue observed_value{};
    Protections::NumericValue threshold_a{};
    Protections::NumericValue threshold_b{};
    bool has_threshold_b{false};
    bool uses_warning_threshold{false};
    float time_window_s{0.0f};
    float sample_rate_hz{0.0f};
};

union DiagnosticPayload {
    RuntimeDiagnosticPayload runtime;
    ProtectionDiagnosticPayload protection;

    constexpr DiagnosticPayload() : runtime{} {}
};

struct DiagnosticRecord {
    Severity severity{Severity::INFO};
    Category category{Category::RUNTIME_WARNING};
    Timestamp timestamp{};
    char origin[Config::origin_capacity + 1]{};
    DiagnosticPayload payload{};
};

class DiagnosticSink {
public:
    virtual ~DiagnosticSink() = default;
    virtual bool publish(const DiagnosticRecord& record) = 0;
};

class DiagnosticFormatter {
public:
    static void describe(const DiagnosticRecord& record, char* buffer, size_t buffer_size);
};

class DiagnosticTimestampProvider {
public:
    static Timestamp capture();
};

class Hub {
public:
    template <typename Sink, typename... Args>
    static expected<Sink*, RegistrationError> emplace_sink(Args&&... args) {
        if (sink_count >= Config::max_sinks) {
            return unexpected(RegistrationError::CAPACITY_EXCEEDED);
        }
        if constexpr (
            sizeof(Sink) > Config::max_sink_storage ||
            alignof(Sink) > alignof(std::max_align_t)) {
            return unexpected(RegistrationError::STORAGE_TOO_SMALL);
        } else {
            SinkStorage& slot = sink_storage[sink_count];
            auto* sink = construct_at(
                reinterpret_cast<Sink*>(slot.bytes.data()),
                std::forward<Args>(args)...
            );
            slot.sink = sink;
            slot.destroy = [](DiagnosticSink* base) { destroy_at(static_cast<Sink*>(base)); };
            sinks[sink_count++] = sink;
            return sink;
        }
    }

    static void publish(DiagnosticRecord record);
    static void publish_runtime_error(
        const char* message,
        bool truncated,
        int line,
        const char* func,
        const char* file
    );
    static void publish_runtime_warning(
        const char* message,
        bool truncated,
        int line,
        const char* func,
        const char* file
    );
    static void publish_protection_event(
        const char* protection_name,
        Protections::RuleState state,
        Protections::RuleEdge edge,
        const Protections::RuleSnapshot& snapshot
    );
    static void flush();

    static void clear_for_testing();
    static size_t history_size_for_testing();
    static size_t pending_size_for_testing();

private:
    struct PendingRecord {
        DiagnosticRecord record{};
        uint8_t delivered_mask{0};
    };

    struct SinkStorage {
        alignas(std::max_align_t) array<byte, Config::max_sink_storage> bytes{};
        DiagnosticSink* sink{nullptr};
        void (*destroy)(DiagnosticSink*){nullptr};

        void reset() {
            if (sink != nullptr && destroy != nullptr) {
                destroy(sink);
            }
            sink = nullptr;
            destroy = nullptr;
        }
    };

    static void push_history(const DiagnosticRecord& record);
    static void push_pending(const DiagnosticRecord& record);
    static void remove_pending(size_t index);

    static array<SinkStorage, Config::max_sinks> sink_storage;
    static array<DiagnosticSink*, Config::max_sinks> sinks;
    static size_t sink_count;
    static array<DiagnosticRecord, Config::history_capacity> history;
    static size_t history_count;
    static size_t history_next_index;
    static array<PendingRecord, Config::pending_capacity> pending_records;
    static size_t pending_count;
};

class Runtime {
public:
    static void install_default_sinks();
    static void reset_for_testing();

private:
    static bool defaults_installed;
};

} // namespace Diagnostics
