#include "HALAL/Services/InfoWarning/InfoWarning.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

namespace {

void publish_runtime_diagnostic(
    Diagnostics::Severity severity,
    const std::source_location& location,
    const char* format,
    va_list arguments
) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments_copy);
    va_end(arguments_copy);

    switch (severity) {
    case Diagnostics::Severity::WARNING:
        Diagnostics::Hub::publish_runtime_warning(
            buffer,
            written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
            static_cast<int>(location.line()),
            location.function_name(),
            location.file_name()
        );
        return;
    case Diagnostics::Severity::INFO:
        Diagnostics::Hub::publish_runtime_info(
            buffer,
            written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
            static_cast<int>(location.line()),
            location.function_name(),
            location.file_name()
        );
        return;
    case Diagnostics::Severity::FAULT:
        std::unreachable();
    }
}

} // namespace

void RuntimeDiagnosticReporter::TriggerWarning(
    const std::source_location& location,
    const char* format,
    ...
) {
    va_list arguments;
    va_start(arguments, format);
    publish_runtime_diagnostic(Diagnostics::Severity::WARNING, location, format, arguments);
    va_end(arguments);
}

void RuntimeDiagnosticReporter::TriggerInfo(
    const std::source_location& location,
    const char* format,
    ...
) {
    va_list arguments;
    va_start(arguments, format);
    publish_runtime_diagnostic(Diagnostics::Severity::INFO, location, format, arguments);
    va_end(arguments);
}

void RuntimeDiagnosticReporter::Flush() { Diagnostics::Hub::flush(); }
