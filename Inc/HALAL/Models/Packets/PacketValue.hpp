#pragma once

#include "../../../C++Utilities/CppUtils.hpp"

template<class ConversionType>
class PacketValue{
public:
    using value_type = ConversionType;
    double* src;
    double factor;

    PacketValue();
    PacketValue(double* src, double factor);

    ConversionType convert();

    void load(ConversionType new_data);

    size_t size();
};
