/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

/**
 * @brief SPI data pakcet class. This class contains a pointer to the data stored
 * 	 	  in bytes and the number of bytes.
 * 
 */
class SPIPacket
{
	uint32_t size;
    uint8_t* data;


private:
	SPIPacket(){};

public:

	/**
	 * @brief Construct a new SPIPacket object. The constructor will handle automaticly
	 * 		  the data type and size automatically.
	 * 
	 * @tparam Type Generic data type.
	 * @param data Data to be stored in the packet.
	 */
	template<typename Type>
	SPIPacket(Type& data) {
		this->size = sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
	
		memcpy(this->data, &data, this->size);
	}

	/**
	 * @brief Construct a new SPIPacket object. Create a SPIPacket by passing an array
	 * 		  of any type and length.
	 * 
	 * @tparam Type Generic data type.
	 * @param data Pointer to the data to be store.
	 * @param size Size of the array.
	 */
	template<typename Type>
	SPIPacket(Type* data, uint32_t size){
		this->size = size * sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
		memcpy(this->data, data, this->size);
	}

	/**
	 * @brief Destroy the SPIPacket object.
	 * 
	 */
	~SPIPacket() {
		if (data)
			free(data);
	}

	/**
	 * @brief Get the data size in bytes.
	 * 
	 * @return uint32_t Returns the data size in bytes.
	 */
	uint32_t get_size() {
		return size;
	}

	/**
	 * @brief Get the pointer to the data.
	 * 
	 * @return uint8_t* Returns a pointer to the packet data.
	 */
	uint8_t* get_data() {
		return data;
	}
};
