#pragma once

#include "../../../C++Utilities/CppUtils.hpp"

template<class ConversionType>
class PacketValue{
public:
    using value_type = ConversionType;
    double* src;
    double factor;

    PacketValue();
    PacketValue(double* src, double factor):src(src),factor(factor){}

    ConversionType convert() {
    	return static_cast<ConversionType>((*src) * factor);
    }

    void load(ConversionType new_data) {
        *src = static_cast<double>(new_data / factor);
    }

    size_t size() {
    	return sizeof(ConversionType);
    }
};
