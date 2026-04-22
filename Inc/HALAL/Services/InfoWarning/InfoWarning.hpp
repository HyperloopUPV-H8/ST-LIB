#pragma once

#include <source_location>

#include "C++Utilities/CppUtils.hpp"

class RuntimeDiagnosticReporter {
public:
    static void TriggerWarning(const std::source_location& location, const char* format, ...);
    static void TriggerInfo(const std::source_location& location, const char* format, ...);
    static void Flush();
};

#define WARNING(x, ...)                                                                            \
    do {                                                                                           \
        RuntimeDiagnosticReporter::TriggerWarning(                                                 \
            std::source_location::current(),                                                       \
            x __VA_OPT__(, ) __VA_ARGS__                                                           \
        );                                                                                         \
    } while (0)

#define INFO(x, ...)                                                                               \
    do {                                                                                           \
        RuntimeDiagnosticReporter::TriggerInfo(                                                    \
            std::source_location::current(),                                                       \
            x __VA_OPT__(, ) __VA_ARGS__                                                           \
        );                                                                                         \
    } while (0)
