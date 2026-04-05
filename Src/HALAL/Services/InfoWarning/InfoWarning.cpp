/*
 * InfoWarning.cpp
 *
 *  Created on: Jun 12, 2024
 *      Author: gonzalo
 */

#include "HALAL/Services/InfoWarning/InfoWarning.hpp"

#include "HALAL/Services/Diagnostics/Diagnostics.hpp"

int InfoWarning::line = 0;
const char* InfoWarning::func = "Warning-No-Func-Found";
const char* InfoWarning::file = "Warning-No-File-Found";

void InfoWarning::SetMetaData(int line, const char* func, const char* file) {
    InfoWarning::line = line;
    InfoWarning::func = func;
    InfoWarning::file = file;
}

void InfoWarning::InfoWarningTrigger(const char* format, ...) {
    char buffer[Diagnostics::Config::runtime_message_capacity + 1]{};
    va_list arguments;
    va_start(arguments, format);
    const int32_t written = vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    Diagnostics::Hub::publish_runtime_warning(
        buffer,
        written < 0 || static_cast<size_t>(written) >= sizeof(buffer),
        line,
        func,
        file
    );
}

void InfoWarning::InfoWarningUpdate() { Diagnostics::Hub::flush(); }
