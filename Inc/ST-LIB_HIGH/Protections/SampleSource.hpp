#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionConcepts.hpp"

template <Protections::ProtectionSample T> class SampleSource {
public:
    using value_type = T;

    explicit constexpr SampleSource(T& value) : value_ptr(&value) {}
    explicit constexpr SampleSource(T* value_ptr) : value_ptr(value_ptr) {}

    constexpr const T& read() const { return *value_ptr; }
    constexpr T* raw() const { return value_ptr; }

private:
    T* value_ptr{nullptr};
};
