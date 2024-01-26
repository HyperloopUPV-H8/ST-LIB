/*
 * SpiPacket.hpp
 *
 *  Created on: 26 jan. 2024
 *      Author: ricardo
 */


#include "SPIPacket.hpp"

#define SPI_ID_SIZE 2
#define PAYLOAD_OVERHEAD SPI_ID_SIZE

#define NO_ORDER_ID 0
#define ERROR_ORDER_ID 1
#define CASTED_ERROR_ORDER_ID (uint16_t) 0x100000000


/**
 * @brief a Base for all Order classes. Any Order class can be stored on a pointer of this type.
 */
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
	void(*callback)(void) = nullptr; /**< callback function called at the end of the Order transaction*/


	/**
	 * @brief function used to change the callback function of the Order
	 */
    virtual void set_callback(void(*callback)(void)) {
        this->callback = callback;
    }

    virtual void master_prepare_buffer(){}/**< function used in SPI callback*/
    virtual void slave_prepare_buffer(){}/**< function used in SPI callback*/

    virtual void master_process_callback(){/**< function used in SPI callback*/
    	 if (callback != nullptr) callback();
    }

    virtual void slave_process_callback(){/**< function used in SPI callback*/
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
		MISO_payload[0] = (uint8_t) id;
		MISO_payload[1] = (uint8_t) (id>>8);
		MOSI_payload[0] = (uint8_t) id;
		MOSI_payload[1] = (uint8_t) (id>>8);
		SPIBaseOrder::SPIOrdersByID[id] = this;
	}

	SPIBaseOrder(SPIBaseOrder& baseOrder) = default;
	virtual ~SPIBaseOrder();

};



/**
 * @brief An order that simply sends the binary data inside the data arrays
 */
class SPIBinaryOrder : public SPIBaseOrder{
public:
	uint8_t* master_array;
	uint8_t* slave_array;

	SPIBinaryOrder(uint16_t id, uint16_t master_data_size, uint16_t slave_data_size) : SPIBaseOrder(id, master_data_size, slave_data_size)
	{
		master_array = SPIBaseOrder::master_data;
		slave_array = SPIBaseOrder::slave_data;
	}
};



/**
 * @brief SPIWordOrder works like SPIBinaryOrder but using full words of data instead of bytes
 *
 * While SPIWordOrder could be emulated easily by the SPIStackOrder, the use of this specific case is
 * common enough to have interest on increasing the efficiency of its execution
 * This version is simply faster than an SPIStackOrder of words as it doesn t need to copy or prepare buffers.
 * The tradeoff is that it lacks the comodity of reading pointers to external variables
 * and the values have to be copied inside of the master_array and slave_array, specifically.
 * Copying into the master_data or slave_data will result in the data being send in bytes instead of words,
 * which will result in undesired data as the memory on stm microcontrollers is on Big endian.
 * Its existence is also a warning on the effects of Big endian when sending buffers of non size 1 data.
 */
class SPIWordOrder : public SPIBaseOrder{
public:
	uint32_t* master_array;
	uint32_t* slave_array;

	SPIWordOrder(uint16_t id, uint16_t master_word_count, uint16_t slave_word_count) : SPIBaseOrder(id, 4*master_word_count, 4*slave_word_count)
	{
		master_array = (uint32_t*)SPIBaseOrder::master_data;
		slave_array = (uint32_t*)SPIBaseOrder::slave_data;
	}
};



/**
 * @brief version of the SPIOrder that receives packet structures and uses the defined variables in there to communicate
 *
 * To use it the user will need to code first the SPIPackets for the master and slave and then give those structures
 * in the constructor of this class. The class will access to the pointers defined inside the Packets and will use them
 * to store received data and send its values.
 * A master comunicator will perform read operations on the master_packet and write on the slave_packet,
 * while the slave will perform reads on slave_packet to send the values inside of it, and will write on the master_packet variables.
 */
class SPIStackOrder : public SPIBaseOrder{
public:
	SPIBasePacket& master_packet;
	SPIBasePacket& slave_packet;

	SPIStackOrder(uint16_t id, SPIBasePacket& master_packet, SPIBasePacket& slave_packet) :
	master_packet(master_packet), slave_packet(slave_packet),
	SPIBaseOrder(id, (uint16_t)master_packet.size, (uint16_t)slave_packet.size){

	}

	void master_prepare_buffer() override{
		memcpy(SPIBaseOrder::master_data, master_packet.build(), master_packet.size);
	}

	void slave_prepare_buffer() override{
		memcpy(SPIBaseOrder::slave_data, slave_packet.build(), slave_packet.size);
	}

	void master_process_callback() override{
		slave_packet.parse(SPIBaseOrder::slave_data);
		if (callback != nullptr) callback();
	}

	void slave_process_callback() override{
		master_packet.parse(SPIBaseOrder::master_data);
		if (callback != nullptr) callback();
	}

};
