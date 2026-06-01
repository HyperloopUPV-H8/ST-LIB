#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionConcepts.hpp"

template <typename Storage>
    requires Protections::ProtectionSample<std::remove_cvref_t<Storage>>
class ReferenceSampleSource {
public:
    using value_type = std::remove_cvref_t<Storage>;

    explicit constexpr ReferenceSampleSource(Storage& value) : value_ptr(&value) {}
    explicit constexpr ReferenceSampleSource(Storage* value_ptr) : value_ptr(value_ptr) {}

    constexpr value_type read() const { return *value_ptr; }
    constexpr Storage* raw() const { return value_ptr; }

private:
    Storage* value_ptr{nullptr};
};

template <typename Storage> using SampleSource = ReferenceSampleSource<Storage>;
