#pragma once
#include <cstddef>

#include "Control/Blocks/MovingAverage.hpp"
#include "Sensors/Common/ADCSensor.hpp"

template <size_t N> class PT100 : protected ST_LIB::Sensors::ADCValueSensor<float> {
public:
    static constexpr float k = 841.836735;
    static constexpr float offset = -492.204082;
    static constexpr float reference_voltage = ST_LIB::Sensors::kDefaultADCReferenceVoltage;
    MovingAverage<N>* filter = nullptr;

    PT100(ST_LIB::ADCDomain::Instance& adc, float* value, MovingAverage<N>& filter);
    PT100(ST_LIB::ADCDomain::Instance& adc, float* value);

    PT100(ST_LIB::ADCDomain::Instance& adc, float& value, MovingAverage<N>& filter);
    PT100(ST_LIB::ADCDomain::Instance& adc, float& value);

    void read();

protected:
    using Base = ST_LIB::Sensors::ADCValueSensor<float>;
};

template <size_t N>
PT100<N>::PT100(ST_LIB::ADCDomain::Instance& adc, float* value, MovingAverage<N>& filter)
    : Base(adc, value), filter(&filter) {}

template <size_t N>
PT100<N>::PT100(ST_LIB::ADCDomain::Instance& adc, float& value, MovingAverage<N>& filter)
    : Base(adc, &value), filter(&filter) {}

template <size_t N>
PT100<N>::PT100(ST_LIB::ADCDomain::Instance& adc, float* value) : Base(adc, value) {}

template <size_t N>
PT100<N>::PT100(ST_LIB::ADCDomain::Instance& adc, float& value) : Base(adc, &value) {}

template <size_t N> void PT100<N>::read() {
    if (!this->is_configured()) {
        return;
    }

    const float val = this->read_voltage(reference_voltage);
    if (filter != nullptr) {
        filter->input(k / val + offset);
        filter->execute();
        *this->value = filter->output_value;
    } else {
        *this->value = k / val + offset;
    }
}
