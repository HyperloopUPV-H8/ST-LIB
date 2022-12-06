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

forward_list<uint8_t> SPI::id_manager = { 0, 1, 2, 3, 4, 5, 6, 7, 	8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

unordered_map<uint8_t, SPI::Instance* > SPI::registered_spi;

unordered_map<SPI::Peripheral, SPI::Instance*> SPI::available_spi = {
	{SPI::spi3, &SPI::instance3}
};


optional<uint8_t> SPI::inscribe(SPI::Peripheral& spi){
	if ( !SPI::available_spi.contains(spi)){
		return nullopt; //TODO: Error handler or throw the exception
	}

	SPI::Instance* spi_instance = SPI::available_spi[spi];

    Pin::inscribe(spi_instance->SCK, ALTERNATIVE);
    Pin::inscribe(spi_instance->MOSI, ALTERNATIVE);
    Pin::inscribe(spi_instance->MISO, ALTERNATIVE);
    Pin::inscribe(spi_instance->SS, ALTERNATIVE);

    uint8_t id = SPI::id_manager.front();

    SPI::registered_spi[id] = spi_instance;

    SPI::id_manager.pop_front();

    return id;
}

optional<uint8_t> SPI::inscribe(SPI::Peripheral& spi, uint32_t data_size){

	optional<uint8_t> id = SPI::inscribe(spi);

	if (id.has_value()) {
		SPI::Instance* spi = SPI::registered_spi[id.value()];

		spi->data_size = data_size;
	}

	return id.value();
}

void SPI::start(){
	for_each(SPI::registered_spi.begin(), SPI::registered_spi.end(),
			[](pair<uint8_t, SPI::Instance*> iter) { SPI::init(iter.second); }
	);
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

