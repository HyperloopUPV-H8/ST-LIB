/*
 * SpiPacket.hpp
 *
 *  Created on: 15 nov. 2023
 *      Author: ricardo
 */

#pragma once

#include "ErrorHandler/ErrorHandler.hpp"
#include "PacketValue.hpp"


#define SPI_ID_SIZE 2
#define PAYLOAD_OVERHEAD SPI_ID_SIZE

#define NO_ORDER_ID 0
#define ERROR_ORDER_ID 1


class SPIBaseOrder{
public:
	static map<uint16_t, SPIBaseOrder*> SPIOrdersByID;


	uint16_t id; /**< Number of the Order ID, saved on the first two bytes of both payloads*/
	uint8_t* MISO_payload; /**< Byte Buffer for the slave DMA output*/
	uint8_t* MOSI_payload; /**< Byte Buffer for the master DMA output*/
	uint16_t payload_size; /**< Size in bytes of both DMA buffers (both have to be the same)*/
	uint8_t* master_data; /**< Pointer to the master Byte buffer after the header (the space where the data is saved)*/
	uint16_t master_data_size;
	uint8_t* slave_data; /**< Pointer to the slave Byte buffer after the header (the space where the data is saved)*/
	uint16_t slave_data_size;
	uint16_t greater_data_size;
	void(*callback)(void) = nullptr;


    virtual void set_callback(void(*callback)(void)) {
        this->callback = callback;
    }

    virtual void master_prepare_buffer(){}
    virtual void slave_prepare_buffer(){}

    virtual void master_process_callback(){
    	 if (callback != nullptr) callback();
    }

    virtual void slave_process_callback(){
    	 if (callback != nullptr) callback();
    }

protected:
	SPIBaseOrder(uint16_t id, uint16_t master_data_size, uint16_t slave_data_size) :  id(id), master_data_size(master_data_size), slave_data_size(slave_data_size){
		if(id == 0){
			ErrorHandler("Cannot use 0 as the SPIOrderID, as it is reserved to the no Order ready signal");
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
		MISO_payload[0] = (uint8_t) id; //this is the only instance where the slave casts Big Endian to Little Endian, as it can preproccess it while the master cant.
		MISO_payload[1] = (uint8_t) (id>>8);
		MOSI_payload[0] = (uint8_t) id;
		MOSI_payload[1] = (uint8_t) (id>>8);
		SPIBaseOrder::SPIOrdersByID[id] = this;
	}

	SPIBaseOrder(SPIBaseOrder& baseOrder) = default;
	virtual ~SPIBaseOrder();

};

class SPIBinaryOrder : public SPIBaseOrder{
public:
	/**
	 * @brief constructor of SPIOrder. The SPIOrder uses the first byte of the data arrays to coordinate the Order id, and builds the arrays with 1 more byte of space.
	 *
	 */
	SPIBinaryOrder(uint16_t id, uint16_t master_data_size, uint16_t slave_data_size) : SPIBaseOrder(id, master_data_size, slave_data_size)
	{}

};

class SPIWordOrder : public SPIBaseOrder{
public:
	/**
	 * @brief constructor of SPIOrder. The SPIOrder uses the first byte of the data arrays to coordinate the Order id, and builds the arrays with 1 more byte of space.
	 *
	 */
	uint32_t* master_array;
	uint32_t* slave_array;

	SPIWordOrder(uint16_t id, uint16_t master_word_count, uint16_t slave_word_count) : SPIBaseOrder(id, 4*master_word_count, 4*slave_word_count)
	{
		master_array = (uint32_t*)SPIBaseOrder::master_data;
		slave_array = (uint32_t*)SPIBaseOrder::slave_data;
	}

};

/*
template<size_t BufferLength,class... Types> requires NotCallablePack<Types*...>
class SPIStackOrder : public SPIBaseOrder{
	OrderValue<>* values[sizeof...(Types)];

	SPIStackOrder(){
        for (OrderValue<>* value : values) {
        	value->set_pointer();
        }
	}

};*/


