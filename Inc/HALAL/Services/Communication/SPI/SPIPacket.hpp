#pragma once
#include "C++Utils.h"


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
	
		
		memccpy(this->data, &data, this->size);
		
		
		
		
	}
	template<typename Type>
	SPIPacket(Type* data, uint32_t size){
		this->size = sizeof(size) * sizeof(Type);
		this->data = (uint8_t*)malloc(this->size);
		memcpy(this->data, data, this->size);
	}

	~SPIPacket() {
		if (data)
		{
			free(data);
		}
		
	}

	uint32_t get_size() {
		return size;
	}

	uint8_t* get_data() {
		return data;
	}
};
