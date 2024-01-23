/*
 * SpiPacket.hpp
 *
 *  Created on: 15 nov. 2023
 *      Author: ricardo
 */

#pragma once

#include "ErrorHandler/ErrorHandler.hpp"


#define SPI_ID_SIZE 2
#define PAYLOAD_OVERHEAD SPI_ID_SIZE

#define NO_PACKET_ID 0
#define ERROR_PACKET_ID 1


class SPIPacketBase{

};

class SPIPacket : SPIPacketBase{
public:
	static map<uint16_t, SPIPacketBase*> SPIPacketsByID;
	uint16_t id;
	uint8_t* MISO_payload;
	uint8_t* MOSI_payload;
	uint16_t payload_size;
	uint8_t* master_data;
	uint16_t master_data_size;
	uint8_t* slave_data;
	uint16_t slave_data_size;
	uint16_t greater_data_size;
    void(*callback)(void) = nullptr;

	/**
	 * @brief constructor of SPIPacket. The SPIPacket uses the first byte of the data arrays to coordinate the packet id, and builds the arrays with 1 more byte of space.
	 *
	 */
	SPIPacket(uint16_t id, uint16_t master_data_size, uint16_t slave_data_size) : id(id), master_data_size(master_data_size), slave_data_size(slave_data_size){
		if(id == 0){
			ErrorHandler("Cannot use 0 as the SPIPacketID, as it is reserved to the no packet ready signal");
		}
		if(master_data_size > slave_data_size){
			payload_size = master_data_size+PAYLOAD_OVERHEAD;
		}else{
			payload_size = slave_data_size+PAYLOAD_OVERHEAD;
		}
		MISO_payload = new uint8_t[payload_size]{0};
		MOSI_payload = new uint8_t[payload_size]{0};
		master_data = &MOSI_payload[PAYLOAD_OVERHEAD];
		slave_data = &MISO_payload[PAYLOAD_OVERHEAD];
		MISO_payload[0] = (uint8_t) id;
		MISO_payload[1] = (uint8_t) (id>>8);
		MOSI_payload[0] = (uint8_t) id;
		MOSI_payload[1] = (uint8_t) (id>>8);
		SPIPacket::SPIPacketsByID[id] = this;
	}


    void set_callback(void(*callback)(void)) {
        this->callback = callback;
    }

    void process_callback(){
    	 if (callback != nullptr) callback();
    }
};


