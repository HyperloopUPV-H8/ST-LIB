/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/SPI/SPI.hpp"

#ifdef HAL_SPI_MODULE_ENABLED


unordered_map<uint8_t, SPI::Instance* > SPI::registered_spi;

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
#define SPI3_CSS_Pin GPIO_PIN_0
#define SPI3_CSS_GPI	O_Port GPIOD

bool SPI::transmit_next_packet(uint8_t id, RawPacket& packet){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    SPI::Instance* spi = SPI::registered_spi[id];

    HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::OFF);
    if (HAL_SPI_Transmit(spi->hspi, packet.get_data(), packet.get_size(), 10) != HAL_OK){
    	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);
        return false; //TODO: Warning, Error during transmision
    }
    HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);

    return true;
}

bool SPI::receive_next_packet(uint8_t id, RawPacket& packet){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    SPI::Instance* spi = SPI::registered_spi[id];

    *packet.get_data() = 0;

    HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::OFF);
    if (HAL_SPI_Receive(spi->hspi, packet.get_data(), packet.get_size(), 10) != HAL_OK) {
    	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);
        return false; //TODO: Warning, Error during receive
    }
    HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);

    spi->receive_ready = false;
    return true;
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
#endif

