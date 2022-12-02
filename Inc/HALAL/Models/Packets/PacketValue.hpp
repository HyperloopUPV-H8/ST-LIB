#pragma once

#include "C++Utilities/CppUtils.hpp"

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

template<class ConversionType>
PacketValue<ConversionType>::PacketValue() = default;

template<class ConversionType>
PacketValue<ConversionType>::PacketValue(double* src, double factor):src(src),factor(factor !=0 ? factor : 1){}

template<class ConversionType>
ConversionType PacketValue<ConversionType>::convert() {
	return static_cast<ConversionType>((*src) * factor);
}

template<class ConversionType>
void PacketValue<ConversionType>::load(ConversionType new_data) {
    *src = static_cast<double>(new_data / factor);
}

template<class ConversionType>
size_t PacketValue<ConversionType>::size() {
	return sizeof(ConversionType);
}
