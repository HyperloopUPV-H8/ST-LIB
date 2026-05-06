#include "ErrorHandler/ErrorHandler.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"

namespace {

struct RuntimeFatalMessage {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    bool truncated{false};
};

RuntimeFatalMessage format_runtime_fatal_message(const char* format, va_list arguments) {
    RuntimeFatalMessage message{};
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    const int32_t written =
        vsnprintf(message.buffer, sizeof(message.buffer), format, arguments_copy);
    va_end(arguments_copy);
    message.truncated = written < 0 || static_cast<size_t>(written) >= sizeof(message.buffer);
    return message;
}

} // namespace

void FaultReporter::Trigger(const std::source_location& location, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    const RuntimeFatalMessage message = format_runtime_fatal_message(format, arguments);
    va_end(arguments);

    FaultController::request_fault(FaultCause::runtime_fault(
        message.buffer,
        message.truncated,
        static_cast<int>(location.line()),
        location.function_name(),
        location.file_name()
    ));
}

void PanicReporter::Trigger(const std::source_location& location, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    const RuntimeFatalMessage message = format_runtime_fatal_message(format, arguments);
    va_end(arguments);

    FaultController::request_fault(FaultCause::panic(
        message.buffer,
        message.truncated,
        static_cast<int>(location.line()),
        location.function_name(),
        location.file_name()
    ));
}

void PanicReporter::Flush() { Diagnostics::Hub::flush(); }
void FaultReporter::Flush() { Diagnostics::Hub::flush(); }
