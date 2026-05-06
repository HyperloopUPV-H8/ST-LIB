#include <gtest/gtest.h>

#include <source_location>

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "HALAL/Services/InfoWarning/InfoWarning.hpp"

namespace ST_LIB::TestPanicReporter {
bool fail_on_error = true;
int call_count = 0;

void reset() {
    fail_on_error = true;
    call_count = 0;
}

void set_fail_on_error(bool enabled) { fail_on_error = enabled; }
} // namespace ST_LIB::TestPanicReporter

void PanicReporter::Trigger(const std::source_location& location, const char* format, ...) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    va_list arguments;
    va_start(arguments, format);
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    ST_LIB::TestPanicReporter::call_count++;
    FaultController::request_fault(FaultCause::panic(
        buffer,
        written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
        static_cast<int>(location.line()),
        location.function_name(),
        location.file_name()
    ));
    if (ST_LIB::TestPanicReporter::fail_on_error) {
        EXPECT_EQ(1, 0);
    }
}

void PanicReporter::Flush() {}

void FaultReporter::Trigger(const std::source_location& location, const char* format, ...) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    va_list arguments;
    va_start(arguments, format);
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    FaultController::request_fault(FaultCause::runtime_fault(
        buffer,
        written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
        static_cast<int>(location.line()),
        location.function_name(),
        location.file_name()
    ));
}

void FaultReporter::Flush() {}
