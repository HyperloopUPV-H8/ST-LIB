/*
 * SpiPacket.hpp
 *
 *  Created on: 15 nov. 2023
 *      Author: ricardo
 */

#pragma once

#include "ErrorHandler/ErrorHandler.hpp"


class SPIPacket{
public:
	static map<uint16_t, SPIPacket*> SPIPacketsByID;
	uint16_t id;
	uint8_t* master_data;
	uint16_t master_data_size;
	uint8_t* slave_data;
	uint16_t slave_data_size;
	uint16_t greater_data_size;

	/**
	 * @brief constructor of SPIPacket. The SPIPacket uses the first byte of the data arrays to coordinate the packet id, and builds the arrays with 1 more byte of space.
	 *
	 */
	SPIPacket(uint16_t id, uint16_t master_data_size, uint16_t slave_data_size) : id(id), master_data_size(master_data_size), slave_data_size(slave_data_size){
		if(id == 0){
			ErrorHandler("Cannot use 0 as the SPIPacketID, as it is reserved to the no packet ready signal");
		}
		if(master_data_size > slave_data_size){
			greater_data_size = master_data_size+2;
		}else{
			greater_data_size = slave_data_size+2;
		}
		master_data = new uint8_t[greater_data_size];
		slave_data = new uint8_t[greater_data_size];
		master_data[0] = (uint8_t) id;
		master_data[1] = (uint8_t) (id>>8);
		slave_data[0] = (uint8_t) id;
		slave_data[1] = (uint8_t) (id>>8);
		SPIPacket::SPIPacketsByID[id] = this;
	}

	SPIPacket(uint16_t id, uint8_t* master_data, uint8_t* slave_data, uint16_t shared_data_size) :
		id(id), master_data(master_data), master_data_size(shared_data_size), slave_data(slave_data), slave_data_size(shared_data_size){
		if(id == 0){
			ErrorHandler("Cannot use 0 as the SPIPacketID, as it is reserved to the no packet ready signal");
		}

	}

};


