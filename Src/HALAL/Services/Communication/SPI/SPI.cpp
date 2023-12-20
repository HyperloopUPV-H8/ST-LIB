/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#include "Communication/SPI/SPI.hpp"

#ifdef HAL_SPI_MODULE_ENABLED


map<uint8_t, SPI::Instance*> SPI::registered_spi {};
map<SPI_HandleTypeDef*, SPI::Instance*> SPI::registered_spi_by_handler {};

uint16_t SPI::id_counter = 0;

uint8_t SPI::inscribe(SPI::Peripheral& spi){
	if ( !SPI::available_spi.contains(spi)){
		ErrorHandler(" The SPI peripheral %d is already used or does not exists.", (uint16_t)spi);

		return 0;
	}

	SPI::Instance* spi_instance = SPI::available_spi[spi];

	spi_instance->SCK->alternative_function = AF5;
	spi_instance->MOSI->alternative_function = AF5;
	spi_instance->MISO->alternative_function = AF5;

    Pin::inscribe(*spi_instance->SCK, ALTERNATIVE);
    Pin::inscribe(*spi_instance->MOSI, ALTERNATIVE);
    Pin::inscribe(*spi_instance->MISO, ALTERNATIVE);

    if(spi_instance->mode == SPI_MODE_MASTER){
        Pin::inscribe(*spi_instance->SS, OUTPUT);
    }

    uint8_t id = SPI::id_counter++;

    DMA::inscribe_stream(spi_instance->hdma_rx);
    DMA::inscribe_stream(spi_instance->hdma_tx);
    SPI::registered_spi[id] = spi_instance;
    SPI::registered_spi_by_handler[spi_instance->hspi] = spi_instance;

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
	 if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return false;
	 }

	SPI::Instance* spi = SPI::registered_spi[id];

	 HAL_StatusTypeDef errorcode = HAL_SPI_Transmit_DMA(spi->hspi, data.data(), data.size());
	 switch(errorcode){
	 	 case HAL_OK:
		 	 return true;
		 break;
	 	 case HAL_BUSY:
	 		 return false;
	 	 break;
	 	 default:
	 		 ErrorHandler("Error while transmiting and receiving with spi DMA. Errorcode %u",(uint8_t)errorcode);
	 		 return false;
	 	 break;
	 }
}

bool SPI::receive(uint8_t id, span<uint8_t> data) {
	 if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return false;
	 }

	SPI::Instance* spi = SPI::registered_spi[id];

	 HAL_StatusTypeDef errorcode = HAL_SPI_Receive_DMA(spi->hspi, data.data(), data.size());
	 switch(errorcode){
	 	 case HAL_OK:
		 	 return true;
		 break;
	 	 case HAL_BUSY:
	 		 return false;
	 	 break;
	 	 default:
	 		 ErrorHandler("Error while transmiting and receiving with spi DMA. Errorcode %u",(uint8_t)errorcode);
	 		 return false;
	 	 break;
	 }
    };

bool SPI::transmit_and_receive(uint8_t id, span<uint8_t> command_data, span<uint8_t> receive_data){
	 if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return false;
	 }

	SPI::Instance* spi = SPI::registered_spi[id];

	 HAL_StatusTypeDef errorcode = HAL_SPI_TransmitReceive_DMA(spi->hspi, command_data.data(), receive_data.data(), command_data.size());
	 switch(errorcode){
	 	 case HAL_OK:
		 	 return true;
		 break;
	 	 case HAL_BUSY:
	 		 return false;
	 	 break;
	 	 default:
	 		 ErrorHandler("Error while transmiting and receiving with spi DMA. Errorcode %u",(uint8_t)errorcode);
	 		 return false;
	 	 break;
	 }

}

bool SPI::master_transmit_packet(uint8_t id, SPIPacket packet){
	 if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return false;
	 }

	SPI::Instance* spi = SPI::registered_spi[id];

	if(spi->state != SPI::IDLE){
		return false;
	}

	spi->state = SPI::STARTING_PACKET;
	spi->SPIPacketID = packet.id;
	SPI::chip_select_off(id);
	HAL_SPI_TransmitReceive_DMA(spi->hspi, (uint8_t *)&spi->SPIPacketID, (uint8_t *)&spi->available_end, 2);
	return true;
}

void SPI::packet_update(){
	for(auto iter: SPI::registered_spi){
		if(iter.second->mode == SPI_MODE_MASTER){
			//if the master is trying to start a packet transaction (he is starting packet and the id of that packet is not 0)
			if(iter.second->SPIPacketID != 0 && iter.second->state == STARTING_PACKET){
				//when the slave available packet is not confirmed to be the same packet id that the master is asking
				if(iter.second->available_end != iter.second->SPIPacketID ){
					//enough time has passed since the last check to ask the slave again if it has the correct packet ID ready
					if(Time::get_global_tick() - iter.second->last_end_check > MASTER_SPI_CHECK_DELAY){
						turn_off_chip_select(iter.second);
						HAL_SPI_TransmitReceive_DMA(iter.second->hspi, (uint8_t *)&iter.second->SPIPacketID, (uint8_t *)&iter.second->available_end, 2);
						iter.second->last_end_check = Time::get_global_tick();
					}
				}
			}
		}
	}
}

void SPI::slave_listen_packets(uint8_t id){
	 if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return;
	 }

	SPI::Instance* spi = SPI::registered_spi[id];

	if(spi->state != SPI::IDLE){
		return;
	}

	spi->state = SPI::WAITING_PACKET;
	spi->available_end = 0;
	HAL_SPI_TransmitReceive_DMA(spi->hspi, (uint8_t *)&spi->available_end, (uint8_t *)&spi->SPIPacketID, 2);
}

void SPI::chip_select_on(uint8_t id){
	if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return;
	}


	SPI::Instance* spi = SPI::registered_spi[id];
	turn_on_chip_select(spi);
}

void SPI::chip_select_off(uint8_t id){
	if (!SPI::registered_spi.contains(id)){
		ErrorHandler("No SPI registered with id %u", id);
		return;
	}

	SPI::Instance* spi = SPI::registered_spi[id];
	turn_off_chip_select(spi);
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
		ErrorHandler("Unable to init %s", spi->name);
		return;
	}

	spi->initialized = true;
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
	if (!SPI::registered_spi_by_handler.contains(hspi)){
		ErrorHandler("Used SPI protocol without the HALAL SPI interface");
		return;
	}

	SPI::Instance* spi = SPI::registered_spi_by_handler[hspi];
	switch(spi->state){
	case SPI::IDLE:
		//Does nothing as there is no packet handling on a direct send
		break;
	case SPI::STARTING_PACKET:
	{
		SPIPacket *packet = SPIPacket::SPIPacketsByID[spi->SPIPacketID];
		if(spi->mode == SPI_MODE_MASTER){ //checks if the packet is ready on slave
			if(spi->available_end == spi->SPIPacketID){
				spi->state = SPI::PROCESSING_PACKET;
				SPI::turn_off_chip_select(spi);
				HAL_SPI_TransmitReceive_DMA(hspi, &packet->master_data[2], &packet->slave_data[2], packet->greater_data_size-2);

			}else{
				spi->last_end_check = Time::get_global_tick();
				SPI::turn_on_chip_select(spi);
			}
		}else{
			ErrorHandler("Used master transmit packet on a slave spi");
		}
		break;
	}
	case SPI::WAITING_PACKET:
	{
		SPIPacket *packet = SPIPacket::SPIPacketsByID[spi->SPIPacketID];
		if(spi->mode == SPI_MODE_SLAVE){ //prepares the packet on the slave
			HAL_SPI_TransmitReceive_DMA(hspi, packet->slave_data, packet->master_data, packet->greater_data_size);
			spi->state = SPI::PROCESSING_PACKET;
		}else{
			ErrorHandler("Used slave process packets on a master spi");
		}
		break;
	}
	case SPI::PROCESSING_PACKET:
		{
			SPIPacket *packet = SPIPacket::SPIPacketsByID[spi->SPIPacketID];
			spi->packet_count++; //counts when a packet has successfully been received.
			if(spi->mode == SPI_MODE_MASTER){ //ends communication
				SPI::turn_on_chip_select(spi);
				spi->state = SPI::IDLE;
			}else{
				spi->state = SPI::WAITING_PACKET; //prepares the next received packet
				spi->SPIPacketID = 0;
				spi->available_end = 0;
				HAL_SPI_TransmitReceive_DMA(spi->hspi, (uint8_t *)&spi->available_end, (uint8_t *)&spi->SPIPacketID, 2);
			}
			break;
		}
	default:
		ErrorHandler("Unknown spi state: %d",spi->state);
		break;
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if((hspi->ErrorCode & HAL_SPI_ERROR_UDR) != 0){

		ErrorHandler("Underrun error, slave is not communicating fast enough");
	}else{
		ErrorHandler("SPI error number %u",hspi->ErrorCode);
	}

}

void SPI::turn_on_chip_select(SPI::Instance* spi) {
	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::ON);
}

void SPI::turn_off_chip_select(SPI::Instance* spi) {
	HAL_GPIO_WritePin(spi->SS->port, spi->SS->gpio_pin, (GPIO_PinState)PinState::OFF);
}



#endif
