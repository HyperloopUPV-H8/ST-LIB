#include "ErrorHandler/ErrorHandler.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"

namespace {

void trigger_runtime_fatal(
    bool panic,
    const std::source_location& location,
    const char* format,
    va_list arguments
) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments);

    if (panic) {
        FaultController::request_fault(FaultCause::panic(
            buffer,
            written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
            static_cast<int>(location.line()),
            location.function_name(),
            location.file_name()
        ));
    } else {
        FaultController::request_fault(FaultCause::runtime_fault(
            buffer,
            written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
            static_cast<int>(location.line()),
            location.function_name(),
            location.file_name()
        ));
    }
}

} // namespace

void FaultReporter::Trigger(const std::source_location& location, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    trigger_runtime_fatal(false, location, format, arguments);
    va_end(arguments);
}

void PanicReporter::Trigger(const std::source_location& location, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    trigger_runtime_fatal(true, location, format, arguments);
    va_end(arguments);
}

void PanicReporter::Flush() { Diagnostics::Hub::flush(); }
void FaultReporter::Flush() { Diagnostics::Hub::flush(); }
