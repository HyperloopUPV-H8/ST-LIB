/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */
#include "Communication/I2C/I2C.hpp"

#ifdef HAL_I2C_MODULE_ENABLED

extern I2C_HandleTypeDef hi2c2;

I2C::Instance I2C::instance2 = { .SCL = PF0, .SDA = PF1, .hi2c = &hi2c2, .instance = I2C2,

                               };

I2C::Peripheral I2C::i2c2 = I2C::Peripheral::peripheral2;

unordered_map<uint8_t, I2C::Instance* > I2C::active_i2c;
unordered_map<I2C::Peripheral, I2C::Instance*> I2C::available_i2cs = {
	{I2C::i2c2, &I2C::instance2}
};

uint16_t I2C::id_counter = 0;

optional<uint8_t> I2C::inscribe(I2C::Peripheral& i2c){
	if ( !I2C::available_i2cs.contains(i2c)){
		return nullopt; //TODO: Error handler or throw the exception
	}

	I2C::Instance* i2c_instance = I2C::available_i2cs[i2c];

    Pin::inscribe(i2c_instance->SCL, ALTERNATIVE);
    Pin::inscribe(i2c_instance->SDA, ALTERNATIVE);


    uint8_t id = I2C::id_counter++;

    I2C::active_i2c[id] = i2c_instance;

    return id;
}


void I2C::start(){
	for(auto iter: I2C::active_i2c){
		I2C::init(iter.second);
	}
}


bool I2C::transmit_next_packet(uint8_t id, I2CPacket& packet){
    if (!I2C::active_i2c.contains(id)){
    	printf("I2C No encontrado al enviar paquete \n\r");
        return false; //TODO: Error handler
    }

    I2C::Instance* i2c = I2C::active_i2c[id];


    if(i2c->hi2c->State == HAL_I2C_STATE_BUSY_TX){
    	printf("I2C Transmit buffer busy!\n\r");
       return false;
    }

    if (HAL_I2C_Master_Transmit_DMA(i2c->hi2c, packet.get_id(), packet.get_data(), packet.get_size()) != HAL_OK){
    	printf("Error during I2C transmission\n\r");
        return false; //TODO: Warning, Error during transmision
    }

    printf("Packet send successfully \n\r");
    return true;
}

bool I2C::receive_next_packet(uint8_t id, I2CPacket& packet){
	 if (!I2C::active_i2c.contains(id))
	        return false; //TODO: Error handler

	 I2C::Instance* i2c = I2C::active_i2c[id];

    if(i2c->hi2c->State == HAL_I2C_STATE_BUSY_RX){
    	printf("I2C Receive buffer busy!\n\r");
       return false;
    }

    *packet.get_data() = 0;

    if (HAL_I2C_Master_Receive_DMA(i2c->hi2c, packet.get_id(), packet.get_data(), packet.get_size()) != HAL_OK) {
    	printf("I2C Error during receive!\n\r");
        return false; //TODO: Warning, Error during receive
    }

    i2c->receive_ready = false;
    return true;
}

void HAL_SPI_RxCpltCallback(I2C_HandleTypeDef * hi2c){
    auto result = find_if(I2C::active_i2c.begin(), I2C::active_i2c.end(), [&](auto i2c){return (i2c.second->hi2c == hi2c);});

    if (result != I2C::active_i2c.end()) {
        (*result).second->receive_ready = true;
    }else{
        //TODO: Warning: Data receive form an unknown SPI
    }
}


bool I2C::read_from(uint8_t id, I2CPacket& packet, uint16_t mem_addr, uint16_t mem_size){
	 if (!I2C::active_i2c.contains(id))
	        return false; //TODO: Error handler

	 I2C::Instance* i2c = I2C::active_i2c[id];


	 *packet.get_data() = 0;

	 if(i2c->hi2c->State == HAL_I2C_STATE_BUSY_TX){
		printf("I2C Transmit buffer busy!\n\r");
		return false;
	 }

	 if (HAL_I2C_Mem_Read_DMA(i2c->hi2c, packet.get_id(), mem_addr, 1, packet.get_data(), packet.get_size())){
		 printf("I2C Error during receive!\n\r");
		 return false; //TODO: Warning, Error during receive
	 }

	 i2c->receive_ready = false;
	 return true;
}

bool I2C::write_to(uint8_t id, I2CPacket& packet, uint16_t mem_addr, uint16_t mem_size){
	if (!I2C::active_i2c.contains(id))
		return false; //TODO: Error handler

	I2C::Instance* i2c = I2C::active_i2c[id];

	 if(i2c->hi2c->State == HAL_I2C_STATE_BUSY_RX){
		printf("I2C Receive buffer busy!\n\r");
	   return false;
	 }

	if (HAL_I2C_Mem_Write_DMA(i2c->hi2c, packet.get_id(), mem_addr, mem_size, packet.get_data(), packet.get_size())){
		return false; //TODO: Warning, Error during transmision
	}

	return true;

}

bool I2C::has_next_packet(uint8_t id){
    if (!I2C::active_i2c.contains(id))
        return false; //TODO: Error handler

    I2C::Instance* i2c = I2C::active_i2c[id];

    return i2c->receive_ready;
}

bool I2C::is_busy(uint8_t id){
    if (!I2C::active_i2c.contains(id))
        return false; //TODO: Error handler

    I2C::Instance* i2c = I2C::active_i2c[id];

    if(i2c->hi2c->State == HAL_I2C_STATE_BUSY_TX){
    	return true;
    }


    return false;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hspi){
    //TODO: Fault, SPI error
    return;
}

/*
void I2C::init(I2C::Instance* spi){
	if (spi->initialized) {
		return;
	}
    SPI_InitTypeDef init_data = spi->hspi->Init;
	spi->hspi->Instance = spi->instance;
	init_data.Mode = spi->mode;
	init_data.Direction = SPI_DIRECTION_2LINES;
	init_data.DataSize = spi->data_size;
	init_data.CLKPolarity = spi->clock_polarity;
	init_data.CLKPhase = spi->clock_phase;
	if (spi->mode == SPI_MODE_MASTER) {
		init_data.NSS = SPI_NSS_HARD_OUTPUT;
		init_data.BaudRatePrescaler = spi->baud_rate_prescaler;
	}else{
		init_data.NSS = SPI_NSS_HARD_INPUT;
	}
	init_data.FirstBit = spi->first_bit;
	init_data.TIMode = SPI_TIMODE_DISABLE;
	init_data.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	init_data.CRCPolynomial = 0x0;
	init_data.NSSPMode = SPI_NSS_PULSE_ENABLE;
	init_data.NSSPolarity = spi->nss_polarity;
	init_data.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	init_data.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	init_data.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	init_data.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	init_data.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	init_data.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	init_data.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	init_data.IOSwap = SPI_IO_SWAP_DISABLE;
	if (HAL_SPI_Init(&hspi3) != HAL_OK){
		//TODO: Error handler
	}
	spi->initialized = true;
}
*/
#endif
