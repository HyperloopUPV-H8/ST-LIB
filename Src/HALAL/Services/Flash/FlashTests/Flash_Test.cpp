/*
 * Flash_Test.cpp
 *
 *  Created on: Oct 20, 2022
 *      Author: Pablo
 */
#include "../../../../../Inc/HALAL/Services/Flash/FlashTests/Flash_Test.hpp"

#define PI 3.1415

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
		uint8_t float_byte_array[4];

		float2Bytes(float_byte_array, PI);
		uint32_t addr = FLASH_SECTOR4_START_ADDRESS;

		return Flash::write((uint32_t *)float_byte_array, addr,(uint32_t) 1);
	}

	bool Test2_Reading_1_float(){
		uint8_t float_byte_array[4];

		Flash::read(FLASH_SECTOR4_START_ADDRESS, (uint32_t *)float_byte_array, 1);

		float result = Bytes2float(float_byte_array);

		return result * 1000 == PI * 1000;
	}

}
