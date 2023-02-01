/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/SPI/SPI.hpp"

#ifdef HAL_SPI_MODULE_ENABLED


unordered_map<uint8_t, SPI::Instance* > SPI::registered_spi = {};

uint16_t SPI::id_counter = 0;

optional<uint8_t> SPI::inscribe(SPI::Peripheral& spi){
	if ( !SPI::available_spi.contains(spi)){
		return nullopt; //TODO: Error handler or throw the exception
	}

	SPI::Instance* spi_instance = SPI::available_spi[spi];

    Pin::inscribe(*spi_instance->SCK, ALTERNATIVE);
    Pin::inscribe(*spi_instance->MOSI, ALTERNATIVE);
    Pin::inscribe(*spi_instance->MISO, ALTERNATIVE);
    Pin::inscribe(*spi_instance->SS, OUTPUT);

    uint8_t id = SPI::id_counter++;

    SPI::registered_spi[id] = spi_instance;

    return id;
}

void SPI::start(){
	for(auto iter: SPI::registered_spi){
		SPI::init(iter.second);
	    HAL_GPIO_WritePin(iter.second->SS->port, iter.second->SS->gpio_pin, (GPIO_PinState)PinState::ON);
	}
}

bool SPI::transmit(uint8_t id, uint8_t data){
	array<uint8_t, 1> data_array = {data};
	return SPI::transmit(id, data_array);
}

bool SPI::transmit(uint8_t id, span<uint8_t> data) {
    if (!SPI::registered_spi.contains(id)) {
        return false; //TODO: Error handler
    }

    SPI::Instance* spi = SPI::registered_spi[id];

    turn_off_chip_select(spi);
    if (HAL_SPI_Transmit(spi->hspi, data.data(), data.size(), 10) != HAL_OK){
    	turn_on_chip_select(spi);
    	return false; //TODO: Warning, Error during transmision
    }
	turn_on_chip_select(spi);

    return true;
}

bool SPI::receive(uint8_t id, span<uint8_t> data) {
        if (!SPI::registered_spi.contains(id))
            return false; //TODO: Error handler

        SPI::Instance* spi = SPI::registered_spi[id];

    	turn_off_chip_select(spi);
        if (HAL_SPI_Receive(spi->hspi, data.data(), data.size(), 10) != HAL_OK) {
        	turn_on_chip_select(spi);
            return false; //TODO: Warning, Error during receive
        }
    	turn_on_chip_select(spi);

        return true;
    };

bool SPI::command_and_receive(uint8_t id, span<uint8_t> command_data, span<uint8_t> receive_data){
	 if (!SPI::registered_spi.contains(id))
	        return false; //TODO: Error handler

	SPI::Instance* spi = SPI::registered_spi[id];

	turn_off_chip_select(spi);
	if (HAL_SPI_Transmit(spi->hspi, command_data.data(), command_data.size(), 10) != HAL_OK){
    	turn_on_chip_select(spi);
		return false; //TODO: Warning, Error during transmision
	}

	if (HAL_SPI_Receive(spi->hspi, receive_data.data(), receive_data.size(), 10) != HAL_OK) {
    	turn_on_chip_select(spi);
		return false; //TODO: Warning, Error during receive
	}
	turn_on_chip_select(spi);

	return true;
}

void SPI::chip_select_on(uint8_t id){
	if (!SPI::registered_spi.contains(id))
		return; //TODO: Error handler

	SPI::Instance* spi = SPI::registered_spi[id];
	turn_on_chip_select(spi);
}

void SPI::chip_select_off(uint8_t id){
	if (!SPI::registered_spi.contains(id))
		return; //TODO: Error handler

	SPI::Instance* spi = SPI::registered_spi[id];
	turn_off_chip_select(spi);
}


void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
    //TODO: Fault, SPI error
    return;
}

void SPI::init(SPI::Instance* spi){
	if (spi->initialized) {
		return;
	}

	spi->hspi->Instance = spi->instance;
	spi->hspi->Init.Mode = spi->mode;
	spi->hspi->Init.Direction = SPI_DIRECTION_2LINES;
	spi->hspi->Init.DataSize = spi->data_size;
	spi->hspi->Init.CLKPolarity = spi->clock_polarity;
	spi->hspi->Init.CLKPhase = spi->clock_phase;

	if (spi->mode == SPI_MODE_MASTER) {
		 spi->hspi->Init.NSS = SPI_NSS_SOFT;
		 spi->hspi->Init.BaudRatePrescaler = spi->baud_rate_prescaler;
	}else{
		 spi->hspi->Init.NSS = SPI_NSS_SOFT;
	}

	spi->hspi->Init.FirstBit = spi->first_bit;
	spi->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	spi->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi->hspi->Init.CRCPolynomial = 0x0;
	spi->hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	spi->hspi->Init.NSSPolarity = spi->nss_polarity;
	spi->hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	spi->hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	spi->hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	spi->hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	spi->hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	spi->hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	spi->hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	spi->hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;


	if (HAL_SPI_Init(spi->hspi) != HAL_OK){
		return;//TODO: Error handler
	}

	spi->initialized = true;
}


void SPI::turn_on_chip_select(SPI::Instance* spi) {
	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);
}

void SPI::turn_off_chip_select(SPI::Instance* spi) {
	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::OFF);
}

#endif
