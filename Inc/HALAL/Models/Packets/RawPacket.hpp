/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

/**
 * @brief Raw data pakcet class. This class contains a pointer to the data stored
 * 	 	  in bytes and the number of bytes.
 *
 */
class RawPacket
{
	uint32_t size; /**< Size in bytes of the data. */
    uint8_t* data; /**< Data pointer. */

private:
    RawPacket(){};

public:
	/**
	 * @brief Construct a new RawPacket object. The constructor will handle automaticly
	 * 		  the data type and size automatically.
	 *
	 * @tparam Type Generic data type.
	 * @param data Data to be stored in the packet.
	 */
	template<typename Type>
	RawPacket(Type& data) {
		this->size = sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);

		memcpy(this->data, &data, this->size);
	}

	/**
	 * @brief Construct a new RawPacket object. Create a RawPacket by passing an array
	 * 		  of any type and length.
	 *
	 * @tparam Type Generic data type.
	 * @param data Pointer to the data to be store.
	 * @param size Size of the array.
	 */
	template<typename Type>
	RawPacket(Type* data, uint32_t size){
		this->size = size * sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
		memcpy(this->data, data, this->size);
	}

	/**
	 * @brief Destroy the RawPacket object.
	 *
	 */
	~RawPacket() {
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
