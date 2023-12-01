/*
 * FMAC.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: Ricardo
 */

#pragma once

#include "ErrorHandler/ErrorHandler.hpp"


#ifdef HAL_FMAC_MODULE_ENABLED

class MultiplierAccelerator{
public:
	enum FMACmodes{
		FIR,
		IIR,
	};

	struct FMACInstance{
		uint16_t *IIRFilterCoeffs;
		uint8_t IIRFiltersSize;
		uint16_t *FFilterCoeffs;
		uint8_t FFiltersSize;
		uint16_t *IIRInValues;
		uint8_t IIRInSize;
		uint16_t *FInValues;
		uint8_t FInSize;
		uint16_t *OutValues;
		uint8_t OutSize;
		FMACmodes mode;
	};

	static FMACInstance Instance;

	static void init(uint8_t IIRFiltersSize, uint8_t FFiltersSize, uint8_t IIRInSize, uint8_t FInSize, uint8_t OutSize);
};

#endif
