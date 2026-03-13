#pragma once

#include "Control/Blocks/MovingAverage.hpp"
#include "LinearSensor.hpp"

template <class Type, size_t N> class FilteredLinearSensor : public LinearSensor<Type> {
    MovingAverage<N>& filter;

public:
    FilteredLinearSensor(
        ST_LIB::ADCDomain::Instance& adc,
        Type slope,
        Type offset,
        Type* value,
        MovingAverage<N>& filter
    )
        : LinearSensor<Type>(adc, slope, offset, value), filter(filter) {}

    FilteredLinearSensor(
        ST_LIB::ADCDomain::Instance& adc,
        Type slope,
        Type offset,
        Type& value,
        MovingAverage<N>& filter
    )
        : LinearSensor<Type>(adc, slope, offset, value), filter(filter) {}

    void read() {
        if (!this->is_configured()) {
            return;
        }

        *this->value =
            filter.compute(this->compute_value_from_voltage(this->read_voltage(this->vref)));
    }
};

// CTAD
#if __cpp_deduction_guides >= 201606
template <class Type, size_t N>
FilteredLinearSensor(
    ST_LIB::ADCDomain::Instance& adc,
    Type slope,
    Type offset,
    Type* value,
    MovingAverage<N>& filter
) -> FilteredLinearSensor<Type, N>;
template <class Type, size_t N>
FilteredLinearSensor(
    ST_LIB::ADCDomain::Instance& adc,
    Type slope,
    Type offset,
    Type& value,
    MovingAverage<N>& filter
) -> FilteredLinearSensor<Type, N>;
#endif
