/*
 * FMAC.hpp
 *
 *  Created on: 30 nov. 2022
 *      Author: Ricardo
 */

#include "FMAC/FMAC.hpp"

MultiplierAccelerator::FMACMemoryLayout MultiplierAccelerator::MemoryLayout;

void MultiplierAccelerator::IIR_software_in_software_out_inscribe(uint8_t IIRFiltersSize, uint8_t FFiltersSize, uint8_t IIRInSize, uint8_t FInSize, uint8_t OutSize){
	MemoryLayout.IIRFilterCoeffs = new int16_t[IIRFiltersSize];
	MemoryLayout.IIRFiltersSize = (uint16_t) IIRFiltersSize;
	MemoryLayout.FFilterCoeffs = new int16_t[FFiltersSize];
	MemoryLayout.FFiltersSize = (uint16_t) FFiltersSize;
	MemoryLayout.IIRInValues = new int16_t[IIRInSize];
	MemoryLayout.IIRInSize = (uint16_t) IIRInSize;
	MemoryLayout.FInValues = new int16_t[FInSize];
	MemoryLayout.FInSize = (uint16_t) FInSize;
	MemoryLayout.OutValues = new int16_t[OutSize];
	MemoryLayout.AuxiliarOutValues = new int16_t[OutSize];
	MemoryLayout.OutSize = (uint16_t) OutSize;
	Instance.mode = IIR;

	MultiplierAccelerator::inscribe();
}

void MultiplierAccelerator::FIR_software_in_software_out_inscribe(uint8_t FFiltersSize, uint8_t FInSize, uint8_t OutSize){
	MemoryLayout.FFilterCoeffs = new int16_t[FFiltersSize]{0};
	MemoryLayout.FFiltersSize = (uint16_t) FFiltersSize;
	MemoryLayout.FInValues = new int16_t[FInSize]{0};
	MemoryLayout.FInSize = (uint16_t) FInSize;
	MemoryLayout.OutValues = new int16_t[OutSize]{0};
	MemoryLayout.AuxiliarOutValues = new int16_t[OutSize]{0};
	MemoryLayout.OutSize = (uint16_t) OutSize;
	Instance.mode = FIR;

	MultiplierAccelerator::inscribe();
}


void MultiplierAccelerator::inscribe(){
	DMA::inscribe_stream(Instance.dma_preload);
	DMA::inscribe_stream(Instance.dma_read);
	DMA::inscribe_stream(Instance.dma_write);
}

void MultiplierAccelerator::start(){
	if(Instance.mode != MultiplierAccelerator::None){
		Instance.hfmac->Instance = FMAC;
		if(HAL_FMAC_Init(Instance.hfmac)!= HAL_OK){
			ErrorHandler("A");
		}

		FMAC_FilterConfigTypeDef sFmacConfig;

		if(Instance.mode == FIR){
			sFmacConfig.CoeffBaseAddress = 0;
			sFmacConfig.CoeffBufferSize = MemoryLayout.FFiltersSize;
			sFmacConfig.InputBaseAddress = MemoryLayout.FFiltersSize;
			sFmacConfig.InputBufferSize = MemoryLayout.FInSize;
			sFmacConfig.InputThreshold = 0;
			sFmacConfig.OutputBaseAddress = MemoryLayout.FFiltersSize + MemoryLayout.FInSize;
			sFmacConfig.OutputBufferSize = MemoryLayout.OutSize;
			sFmacConfig.OutputThreshold = 0;
			sFmacConfig.pCoeffA = NULL;
			sFmacConfig.CoeffASize = 0;
			sFmacConfig.pCoeffB = MemoryLayout.FFilterCoeffs;
			sFmacConfig.CoeffBSize = MemoryLayout.FFiltersSize;
			sFmacConfig.Filter            = FMAC_FUNC_CONVO_FIR;
			sFmacConfig.InputAccess       = FMAC_BUFFER_ACCESS_DMA;
			sFmacConfig.OutputAccess      = FMAC_BUFFER_ACCESS_DMA;
			sFmacConfig.Clip = FMAC_CLIP_ENABLED;
			sFmacConfig.P = MemoryLayout.FFiltersSize;
			sFmacConfig.Q = 0;
			sFmacConfig.R = 0;

			/* Configure the FMAC */
			if (HAL_FMAC_FilterConfig(Instance.hfmac, &sFmacConfig) != HAL_OK){
				ErrorHandler("Error while defining the filter memory layout of the FMAC");
			}

		}else{

		}

	}
}

void MultiplierAccelerator::software_preload(int16_t* preload_data){
	if (HAL_FMAC_FilterPreload(Instance.hfmac, preload_data, (uint8_t) MemoryLayout.FInSize, NULL, 0) != HAL_OK)
	{
		ErrorHandler("");
	}
}

void MultiplierAccelerator::software_preload(int16_t* preload_data, int16_t* preload_secondary_data){
	if (HAL_FMAC_FilterPreload(Instance.hfmac, preload_data, (uint8_t) MemoryLayout.FInSize, preload_secondary_data, (uint8_t) MemoryLayout.IIRInSize) != HAL_OK){
		ErrorHandler("");
	}

}

void MultiplierAccelerator::software_process(int16_t* calculated_data){
	if(HAL_FMAC_FilterStart(Instance.hfmac, calculated_data, &MemoryLayout.OutSize) != HAL_OK){
		ErrorHandler("");
	}
}

void MultiplierAccelerator::software_load(int16_t* input_data){
	if(HAL_FMAC_AppendFilterData(Instance.hfmac, input_data, &MemoryLayout.FInSize)){
		ErrorHandler("");
	}
}

void MultiplierAccelerator::software_end_process(){
	if(HAL_FMAC_FilterStop(Instance.hfmac) != HAL_OK){
		ErrorHandler("");
	}
}


bool MultiplierAccelerator::is_ready(){
	return Instance.hfmac->State == HAL_FMAC_STATE_READY;
}

void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef *hfmac){
	//HAL_FMAC_ConfigFilterOutputBuffer(MultiplierAccelerator::Instance.hfmac, MultiplierAccelerator::MemoryLayout.OutValues, &MultiplierAccelerator::MemoryLayout.OutSize);
}

void HAL_FMAC_GetDataCallback(FMAC_HandleTypeDef *hfmac){

}

