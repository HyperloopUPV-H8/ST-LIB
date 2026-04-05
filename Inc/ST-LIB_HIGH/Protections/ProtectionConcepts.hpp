#pragma once

#include "C++Utilities/CppUtils.hpp"

namespace Protections {

template <typename T>
using remove_cvref_t = std::remove_cvref_t<T>;

template <typename T>
concept ArithmeticSample =
    std::is_arithmetic_v<remove_cvref_t<T>> && !std::same_as<remove_cvref_t<T>, long double>;

template <typename T> concept FloatingSample = std::floating_point<remove_cvref_t<T>>;

template <typename T>
concept ComparableSample =
    ArithmeticSample<T> && requires(remove_cvref_t<T> lhs, remove_cvref_t<T> rhs) {
        { lhs < rhs } -> std::convertible_to<bool>;
        { lhs > rhs } -> std::convertible_to<bool>;
    };

template <typename T>
concept EqualityComparableSample =
    ArithmeticSample<T> && std::equality_comparable<remove_cvref_t<T>>;

template <typename T>
concept SupportedProtectionSample =
    std::same_as<remove_cvref_t<T>, bool> || std::same_as<remove_cvref_t<T>, int8_t> ||
    std::same_as<remove_cvref_t<T>, uint8_t> || std::same_as<remove_cvref_t<T>, int16_t> ||
    std::same_as<remove_cvref_t<T>, uint16_t> || std::same_as<remove_cvref_t<T>, int32_t> ||
    std::same_as<remove_cvref_t<T>, uint32_t> || std::same_as<remove_cvref_t<T>, int64_t> ||
    std::same_as<remove_cvref_t<T>, uint64_t> || std::same_as<remove_cvref_t<T>, float> ||
    std::same_as<remove_cvref_t<T>, double>;

template <typename T> concept ProtectionSample = ArithmeticSample<T> && SupportedProtectionSample<T>;

template <typename Source>
concept ReadableSampleSource =
    requires(const remove_cvref_t<Source>& source) {
        typename remove_cvref_t<Source>::value_type;
        requires ProtectionSample<typename remove_cvref_t<Source>::value_type>;
        { source.read() } -> std::convertible_to<typename remove_cvref_t<Source>::value_type>;
    };

} // namespace Protections
