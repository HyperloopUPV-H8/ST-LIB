/*
 * ErrorHandler.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: Pablo
 */

#include "ErrorHandler/ErrorHandler.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"
#include "ST-LIB_HIGH/Protections/FaultController.hpp"

int ErrorHandlerModel::line = 0;
const char* ErrorHandlerModel::func = "Error-No-Func-Found";
const char* ErrorHandlerModel::file = "Error-No-File-Found";

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

    Diagnostics::Hub::publish_runtime_error(
        buffer,
        written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
        line,
        func,
        file
    );
    FaultController::enter_fault();
}

void ErrorHandlerModel::ErrorHandlerUpdate() { Diagnostics::Hub::flush(); }
