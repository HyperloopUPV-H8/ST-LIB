/*
 * FMAC.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: Ricardo
 */

#include "FMAC/FMAC.hpp"

MultiplierAccelerator::FMACInstance MultiplierAccelerator::Instance;

void MultiplierAccelerator::init(uint8_t IIRFiltersSize, uint8_t FFiltersSize, uint8_t IIRInSize, uint8_t FInSize, uint8_t OutSize){
	__HAL_RCC_FMAC_CLK_ENABLE();
	Instance.IIRFilterCoeffs = new uint16_t[IIRFiltersSize];
	Instance.IIRFiltersSize = IIRFiltersSize;
	Instance.FFilterCoeffs = new uint16_t[FFiltersSize];
	Instance.FFiltersSize = FFiltersSize;
	Instance.IIRInValues = new uint16_t[IIRInSize];
	Instance.IIRInSize = IIRInSize;
	Instance.FInValues = new uint16_t[FInSize];
	Instance.FInSize = FInSize;
	Instance.OutValues = new uint16_t[OutSize];
	Instance.OutSize = OutSize;
}
