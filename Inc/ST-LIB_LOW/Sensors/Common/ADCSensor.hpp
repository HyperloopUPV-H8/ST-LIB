#pragma once

#include <algorithm>
#include <cstddef>

#include "HALAL/Services/ADC/ADC.hpp"

namespace ST_LIB::Sensors {

inline constexpr float kDefaultADCReferenceVoltage = 3.3f;

template <class ValueType> class ADCValueSensor {
protected:
    using ADCInstance = ST_LIB::ADCDomain::Instance;

    constexpr ADCValueSensor() = default;
    constexpr ADCValueSensor(ADCInstance& adc, ValueType* value) : adc(&adc), value(value) {}

    [[nodiscard]] bool is_configured() const { return adc != nullptr && value != nullptr; }

    [[nodiscard]] float read_raw() const { return (adc == nullptr) ? 0.0f : adc->get_raw(); }

    [[nodiscard]] float read_voltage(float vref = kDefaultADCReferenceVoltage) const {
        if (adc == nullptr) {
            return 0.0f;
        }
        return adc->get_value_from_raw(read_raw(), vref);
    }

    [[nodiscard]] float normalized_raw() const {
        if (adc == nullptr) {
            return 0.0f;
        }

        const float max_raw =
            static_cast<float>(ADCInstance::max_raw_for_resolution(adc->resolution));
        if (max_raw <= 0.0f) {
            return 0.0f;
        }

        return read_raw() / max_raw;
    }

    [[nodiscard]] std::size_t bucket_index(std::size_t bucket_count) const {
        if (bucket_count == 0U) {
            return 0U;
        }

        const auto index = static_cast<std::size_t>(normalized_raw() * bucket_count);
        return std::min(index, bucket_count - 1U);
    }

    [[nodiscard]] std::size_t scaled_index(std::size_t max_index) const {
        const auto index = static_cast<std::size_t>(normalized_raw() * max_index);
        return std::min(index, max_index);
    }

    ADCInstance* adc = nullptr;
    ValueType* value = nullptr;
};

} // namespace ST_LIB::Sensors
