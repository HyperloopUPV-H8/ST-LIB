/*
 * FMAC.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: Ricardo
 */

#pragma once

#include "DMA/DMA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"


#ifdef HAL_FMAC_MODULE_ENABLED

class MultiplierAccelerator{
public:
	enum FMACmodes{
		None,
		FIR,
		IIR,

	};


	struct FMACMemoryLayout{
		int16_t *IIRFilterCoeffs;
		uint16_t IIRFiltersSize;
		int16_t *FFilterCoeffs;
		uint16_t FFiltersSize;
		int16_t *IIRInValues;
		uint16_t IIRInSize;
		int16_t *FInValues;
		uint16_t FInSize;
		int16_t *OutValues;
		int16_t *AuxiliarOutValues;
		uint16_t OutSize;
	};

	struct FMACInstance{
		FMACmodes mode;
		FMAC_HandleTypeDef *hfmac;
		DMA::Stream dma_preload;
		DMA::Stream dma_read;
		DMA::Stream dma_write;

	};

	static FMACInstance Instance;
	static FMACMemoryLayout MemoryLayout;

	static void IIR_software_in_software_out_inscribe(uint8_t IIRFiltersSize, uint8_t FFiltersSize, uint8_t IIRInSize, uint8_t FInSize, uint8_t OutSize);
	static void FIR_software_in_software_out_inscribe(uint8_t FFiltersSize, uint8_t FInSize, uint8_t OutSize);
	static void start();
	
	static void software_preload(int16_t* preload_data);
	static void software_preload(int16_t* preload_data, int16_t* preload_secondary_data);
	static void software_process(int16_t* calculated_data, uint16_t* output_size);
	static void software_load(int16_t* input_data, uint16_t* input_size);
	static void software_end_process();
	static bool is_ready();

private:

	/**
	 * @brief generic process used on all inscribe type functions
	*/
	static void inscribe();

	/**
	 * @brief method that configurates the FMAC, used in all init methods
	 * 
	 * @param IIRFiltersCoeffs pointer to the Coefficients used in the IIR mode. Declare them as nullptr on FIR as they will not be used. Known as A coefficients in FMAC HAL peripheral
	 * @param IIRFiltersSize number of IIRCoefficients, must be one less than FFiltersSize in normal IIR use, and 0 on any FIR use
	 * @param FFiltersCoeffs pointer to the Coefficients used in all modes as the external input. 
	 * @param FFiltersSize number of FIRCoefficients, equal to the inputs computated in one calculation
	 * @param 
	*/
	static void configure(int16_t *IIRFiltersCoeffs, uint8_t IIRFiltersSize, int16_t *FFiltersCoeffs, uint8_t FFiltersSize, 
	int16_t *IIRInValues, uint8_t IIRInSize, int16_t *FInValues, uint8_t FInSize, int16_t *OutValues, uint8_t OutSize, FMACmodes mode);
};

#endif
