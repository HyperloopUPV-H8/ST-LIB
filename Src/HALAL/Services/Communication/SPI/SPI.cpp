/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/SPI/SPI.hpp"

#ifdef HAL_SPI_MODULE_ENABLED

SPI::Instance SPI::instance3 = { .SCK = PC10, .MOSI = PB2, .MISO = PC11, .SS = PA4,
                                 .hspi = &hspi3, .instance = SPI3,
								 .baud_rate_prescaler = SPI_BAUDRATEPRESCALER_256,
                               };
SPI::Peripheral SPI::spi3 = SPI::Peripheral::peripheral3;

unordered_map<uint8_t, SPI::Instance* > SPI::registered_spi;
unordered_map<SPI::Peripheral, SPI::Instance*> SPI::available_spi = {
	{SPI::spi3, &SPI::instance3}
};

uint16_t SPI::id_counter = 0;

optional<uint8_t> SPI::inscribe(SPI::Peripheral& spi){
	if ( !SPI::available_spi.contains(spi)){
		return nullopt; //TODO: Error handler or throw the exception
	}

	SPI::Instance* spi_instance = SPI::available_spi[spi];

    Pin::inscribe(spi_instance->SCK, ALTERNATIVE);
    Pin::inscribe(spi_instance->MOSI, ALTERNATIVE);
    Pin::inscribe(spi_instance->MISO, ALTERNATIVE);
    Pin::inscribe(spi_instance->SS, ALTERNATIVE);

    uint8_t id = SPI::id_counter++;

    SPI::registered_spi[id] = spi_instance;

    return id;
}

void SPI::start(){
	for(auto iter: SPI::registered_spi){
		SPI::init(iter.second);
	}
}


bool SPI::transmit_next_packet(uint8_t id, RawPacket& packet){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    SPI::Instance* spi = SPI::registered_spi[id];

    if(spi->hspi->State == HAL_SPI_STATE_BUSY_TX)
       return false;


    if (HAL_SPI_Transmit_IT(spi->hspi, packet.get_data(), packet.get_size()) != HAL_OK){
        return false; //TODO: Warning, Error during transmision
    }

    return true;
}

bool SPI::receive_next_packet(uint8_t id, RawPacket& packet){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    SPI::Instance* spi = SPI::registered_spi[id];

    if(spi->hspi->State == HAL_SPI_STATE_BUSY_RX)
       return false;

    *packet.get_data() = 0;

    if (HAL_SPI_Receive_DMA(spi->hspi, packet.get_data(), packet.get_size()) != HAL_OK) {
        return false; //TODO: Warning, Error during receive
    }

    spi->receive_ready = false;
    return true;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef * hspi){
    auto result = find_if(SPI::registered_spi.begin(), SPI::registered_spi.end(), [&](auto spi){return spi.second->hspi == hspi;});

    if (result != SPI::registered_spi.end()) {
        (*result).second->receive_ready = true;
    }else{
        //TODO: Warning: Data receive form an unknown SPI
    }
}

bool SPI::has_next_packet(uint8_t id){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    auto* spi = SPI::registered_spi[id];

    return spi->receive_ready;
}

bool SPI::is_busy(uint8_t id){
    if (!SPI::registered_spi.contains(id))
        return false; //TODO: Error handler

    SPI::Instance* spi = SPI::registered_spi[id];

    if(spi->hspi->State == HAL_SPI_STATE_BUSY_TX)
       return true;

    return false;
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
		spi->hspi->Init.NSS = SPI_NSS_HARD_OUTPUT;
		spi->hspi->Init.BaudRatePrescaler = spi->baud_rate_prescaler;
	}else{
		spi->hspi->Init.NSS = SPI_NSS_HARD_INPUT;
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


	if (HAL_SPI_Init(&hspi3) != HAL_OK){
		//TODO: Error handler
	}

	spi->initialized = true;
}
#endif

