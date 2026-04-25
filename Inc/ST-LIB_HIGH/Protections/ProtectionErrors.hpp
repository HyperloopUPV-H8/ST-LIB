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

} // namespace Protections
