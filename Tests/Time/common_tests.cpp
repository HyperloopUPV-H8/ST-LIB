#include <gtest/gtest.h>
#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"
#include "ErrorHandler/ErrorHandler.hpp"
#include "HALAL/Services/InfoWarning/InfoWarning.hpp"

int ErrorHandlerModel::line = 0;
const char* ErrorHandlerModel::func = "";
const char* ErrorHandlerModel::file = "";

namespace ST_LIB::TestErrorHandler {
bool fail_on_error = true;
int call_count = 0;

void reset() {
    fail_on_error = true;
    call_count = 0;
}

void set_fail_on_error(bool enabled) { fail_on_error = enabled; }
} // namespace ST_LIB::TestErrorHandler

void ErrorHandlerModel::SetMetaData(int line, const char* func, const char* file) {
    ErrorHandlerModel::line = line;
    ErrorHandlerModel::func = func;
    ErrorHandlerModel::file = file;
}

void ErrorHandlerModel::ErrorHandlerTrigger(const char* format, ...) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    va_list arguments;
    va_start(arguments, format);
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    ST_LIB::TestErrorHandler::call_count++;
    Diagnostics::Hub::publish_runtime_error(
        buffer,
        written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
        line,
        func,
        file
    );
    FaultController::enter_fault();
    if (ST_LIB::TestErrorHandler::fail_on_error) {
        EXPECT_EQ(1, 0);
    }
}

void ErrorHandlerModel::ErrorHandlerUpdate() {}

std::string InfoWarning::line;
std::string InfoWarning::func;
std::string InfoWarning::file;

namespace ST_LIB::TestInfoWarning {
bool fail_on_error = false;
int call_count = 0;

void reset() {
    fail_on_error = false;
    call_count = 0;
}

void set_fail_on_error(bool enabled) { fail_on_error = enabled; }
}; // namespace ST_LIB::TestInfoWarning

void InfoWarning::SetMetaData(int line, const char* func, const char* file) {
    InfoWarning::line = to_string(line);
    InfoWarning::func = string(func);
    InfoWarning::file = string(file);
}

void InfoWarning::InfoWarningTrigger(string format, ...) {
    (void)format;
    ST_LIB::TestInfoWarning::call_count++;
    if (ST_LIB::TestInfoWarning::fail_on_error) {
        EXPECT_EQ(1, 0);
    }
}
