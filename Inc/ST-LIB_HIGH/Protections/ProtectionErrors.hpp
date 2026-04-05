#pragma once

#include "C++Utilities/CppUtils.hpp"

namespace Protections {

enum class RuleConfigError : uint8_t {
    INVALID_WARNING_THRESHOLD = 0,
    INVALID_RANGE_THRESHOLDS,
    INVALID_WINDOW,
    INVALID_SAMPLE_RATE,
    WINDOW_CAPACITY_EXCEEDED,
};

enum class ProtectionError : uint8_t {
    INVALID_HANDLE = 0,
    INVALID_RULE_CONFIGURATION,
    RULE_CAPACITY_EXCEEDED,
    PROTECTION_CAPACITY_EXCEEDED,
    REGISTRATION_LOCKED,
};

} // namespace Protections
