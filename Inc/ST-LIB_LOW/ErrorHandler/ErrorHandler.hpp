#pragma once

#include <source_location>

#include "C++Utilities/CppUtils.hpp"

#ifndef SIM_ON
#include "HALAL/Services/Communication/UART/UART.hpp"
#endif // !defined(SIM_ON)

class PanicReporter {
public:
    static void Trigger(
        const std::source_location& location,
        const char* format,
        ...
    );
    static void Flush();
};

class FaultReporter {
public:
    static void Trigger(
        const std::source_location& location,
        const char* format,
        ...
    );
    static void Flush();
};

#define PANIC(x, ...)                                                                              \
    do {                                                                                           \
        PanicReporter::Trigger(std::source_location::current(), x __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

#define FAULT(x, ...)                                                                              \
    do {                                                                                           \
        FaultReporter::Trigger(std::source_location::current(), x __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

using ErrorHandlerModel = PanicReporter;

// Deprecated compatibility macro.
#define ErrorHandler(x, ...) PANIC(x __VA_OPT__(, ) __VA_ARGS__)
