/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

class SPIPacket
{
	uint32_t size;
    uint8_t* data;

public:

	SPIPacket(){};

	template<typename Type>
	SPIPacket(Type& data) {
		this->size = sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
	
		memcpy(this->data, &data, this->size);
	}

	template<typename Type>
	SPIPacket(Type* data, uint32_t size){
		this->size = sizeof(size) * sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
		memcpy(this->data, data, this->size);
	}

	~SPIPacket() {
		if (data)
			free(data);
	}

	uint32_t get_size() {
		return size;
	}

	uint8_t* get_data() {
		return data;
	}
};
