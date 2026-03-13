/*
 * LinearSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include <type_traits>

#include "Sensors/Common/ADCSensor.hpp"

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
class LinearSensor : protected ST_LIB::Sensors::ADCValueSensor<Type> {
public:
    LinearSensor() = default;
    LinearSensor(
        ST_LIB::ADCDomain::Instance& adc,
        Type slope,
        Type offset,
        Type* value,
        float vref = 3.3f
    );
    LinearSensor(
        ST_LIB::ADCDomain::Instance& adc,
        Type slope,
        Type offset,
        Type& value,
        float vref = 3.3f
    );

    void read();

    void set_offset(Type new_offset);
    [[nodiscard]] Type get_offset() const;

    void set_gain(Type new_gain);
    [[nodiscard]] Type get_gain() const;

    [[nodiscard]] Type* get_value_pointer() const;

protected:
    using Base = ST_LIB::Sensors::ADCValueSensor<Type>;

    [[nodiscard]] Type compute_value_from_voltage(float voltage) const;

    Type slope{};
    Type offset{};
    float vref = ST_LIB::Sensors::kDefaultADCReferenceVoltage;
};

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
LinearSensor<Type>::LinearSensor(
    ST_LIB::ADCDomain::Instance& adc,
    Type slope,
    Type offset,
    Type* value,
    float vref
)
    : Base(adc, value), slope(slope), offset(offset), vref(vref) {}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
LinearSensor<Type>::LinearSensor(
    ST_LIB::ADCDomain::Instance& adc,
    Type slope,
    Type offset,
    Type& value,
    float vref
)
    : LinearSensor(adc, slope, offset, &value, vref) {}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
void LinearSensor<Type>::read() {
    if (!this->is_configured()) {
        return;
    }

    *this->value = compute_value_from_voltage(this->read_voltage(vref));
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
Type LinearSensor<Type>::compute_value_from_voltage(float voltage) const {
    const Type sensor_voltage = static_cast<Type>(voltage);
    return slope * sensor_voltage + offset;
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
Type LinearSensor<Type>::get_offset() const {
    return offset;
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
Type LinearSensor<Type>::get_gain() const {
    return slope;
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
void LinearSensor<Type>::set_offset(Type new_offset) {
    offset = new_offset;
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
void LinearSensor<Type>::set_gain(Type new_gain) {
    slope = new_gain;
}

template <class Type>
    requires std::is_integral_v<Type> || std::is_floating_point_v<Type>
Type* LinearSensor<Type>::get_value_pointer() const {
    return this->value;
}
