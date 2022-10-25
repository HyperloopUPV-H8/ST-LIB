/*
 * Flash_Test.cpp
 *
 *  Created on: Oct 20, 2022
 *      Author: Pablo
 */
#include "../../../../../Inc/HALAL/Services/Flash/FlashTests/Flash_Test.hpp"

namespace FlashTest{

	void float2Bytes(uint8_t * ftoa_bytes_temp, float float_variable){
		union {
		  float a;
		  uint8_t bytes[4];
		} thing;

		thing.a = float_variable;

		for (uint8_t i = 0; i < 4; i++) {
		  ftoa_bytes_temp[i] = thing.bytes[i];
		}
	}

	float Bytes2float(uint8_t * ftoa_bytes_temp){
		union {
		  float a;
		  uint8_t bytes[4];
		} thing;

		for (uint8_t i = 0; i < 4; i++) {
			thing.bytes[i] = ftoa_bytes_temp[i];
		}

	   float float_variable =  thing.a;
	   return float_variable;
	}

	bool Test1_Writing_1_float(){
		float pi = 3.141592;

		//uint32_t addresses[] = {0x00000000, 0x00000010, 0x00000020, 0x00000028};
		uint8_t float_byte_array[4];

		float2Bytes(float_byte_array, pi);
		uint32_t addr = FLASH_SECTOR5_START_ADDRESS; // + addresses[rand()% 4];


		return Flash::write((uint32_t *)float_byte_array, addr,(uint32_t) 1)&
		 Flash::write((uint32_t *)float_byte_array, addr + 0x20,(uint32_t) 1)&
		 Flash::write((uint32_t *)float_byte_array, addr + 0x40,(uint32_t) 1);
	}



}
