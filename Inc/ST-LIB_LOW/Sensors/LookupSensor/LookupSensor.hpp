/*
 * LookupSensor.hpp
 *
 *  Created on: Nov 7, 2022
 *      Author: ricardo
 */

#pragma once
#include <cstddef>
#include <span>

#include "Sensors/Common/ADCSensor.hpp"

class LookupSensor : protected ST_LIB::Sensors::ADCValueSensor<double> {
public:
    LookupSensor() = default;
    LookupSensor(ST_LIB::ADCDomain::Instance& adc, std::span<const double> table, double* value)
        : Base(adc, value), table(table) {}
    LookupSensor(ST_LIB::ADCDomain::Instance& adc, std::span<const double> table, double& value)
        : LookupSensor(adc, table, &value) {}
    LookupSensor(
        ST_LIB::ADCDomain::Instance& adc,
        const double* table,
        std::size_t table_size,
        double* value
    )
        : LookupSensor(
              adc,
              (table == nullptr) ? std::span<const double>{}
                                 : std::span<const double>(table, table_size),
              value
          ) {}
    LookupSensor(
        ST_LIB::ADCDomain::Instance& adc,
        const double* table,
        std::size_t table_size,
        double& value
    )
        : LookupSensor(adc, table, table_size, &value) {}
    void read() {
        if (!this->is_configured() || table.empty()) {
            return;
        }

        *this->value = table[this->bucket_index(table.size())];
    }

protected:
    using Base = ST_LIB::Sensors::ADCValueSensor<double>;

    std::span<const double> table{};
};
