#include <gtest/gtest.h>
#include "ErrorHandler/ErrorHandler.hpp"
#include "HALAL/Services/InfoWarning/InfoWarning.hpp"

std::string ErrorHandlerModel::line;
std::string ErrorHandlerModel::func;
std::string ErrorHandlerModel::file;

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
    ErrorHandlerModel::line = to_string(line);
    ErrorHandlerModel::func = string(func);
    ErrorHandlerModel::file = string(file);
}

void ErrorHandlerModel::ErrorHandlerTrigger(string format, ...) {
    (void)format;
    ST_LIB::TestErrorHandler::call_count++;
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
