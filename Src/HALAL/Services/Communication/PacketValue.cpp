/*
 * PacketValue.cpp
 *
 *  Created on: 26 oct. 2022
 *      Author: stefa
 */

#include <Packets/PacketValue.hpp>

template<class ConversionType>
PacketValue<ConversionType>::PacketValue() = default;

template<class ConversionType>
PacketValue<ConversionType>::PacketValue(double* src, double factor) : src(src), factor(factor) {}

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

