/*
 * Flash_Test.cpp
 *
 *  Created on: Oct 20, 2022
 *      Author: Pablo
 */
#include "Flash_Test.hpp"

namespace FlashTest{
	void float2Bytes(uint8_t * ftoa_bytes_temp,float float_variable)
	{
		union {
		  float a;
		  uint8_t bytes[4];
		} thing;

		thing.a = float_variable;

		for (uint8_t i = 0; i < 4; i++) {
		  ftoa_bytes_temp[i] = thing.bytes[i];
		}

	}

	float Bytes2float(uint8_t * ftoa_bytes_temp)
	{
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

	bool Test1_Wrinting_1_float(){
		float pi = 3.141592;

		uint8_t float_byte_array[4];
		float2Bytes(float_byte_array, pi);
		uint32_t addresses[] = {0x08010000, 0x08010012, 0x08010020, 0x08010028};
		uint32_t addr = addresses[rand()% 4];
		Flash::write((uint32_t *)float_byte_array, (uint32_t)addr,(uint32_t) 1);
		return true;
	}



}
