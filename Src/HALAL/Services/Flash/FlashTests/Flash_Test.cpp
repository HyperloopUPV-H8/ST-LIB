/*
 * Flash_Test.cpp
 *
 *  Created on: Oct 20, 2022
 *      Author: Pablo
 */
#include "../../../../../Inc/HALAL/Services/Flash/FlashTests/Flash_Test.hpp"

#define PI 					3.1415
#define E  					2.7183
#define SQUARE_ROOT_OF_TWO 	1.4142

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

	bool test1_writing_1_float(){
		uint8_t float_byte_array[4];

		float2Bytes(float_byte_array, PI);
		uint32_t addr = FLASH_SECTOR4_START_ADDRESS;

		return Flash::write((uint32_t *)float_byte_array, addr,(uint32_t) 1);
	}

	bool test2_reading_1_float(){
		uint8_t float_byte_array[4];

		Flash::read(FLASH_SECTOR4_START_ADDRESS, (uint32_t *)float_byte_array, 1);

		float result = Bytes2float(float_byte_array);

		return result * 10000 == PI * 10000;
	}

   bool	test3_writing_multiple_float(){
	   uint8_t float_byte_array[4];

	   uint32_t addr = FLASH_SECTOR4_START_ADDRESS;
	   uint32_t offset1 = 0x04;
	   uint32_t offset2 = 0x20;

	   float2Bytes(float_byte_array, PI);
	   Flash::write((uint32_t *)float_byte_array, addr,(uint32_t) 1);
	   float2Bytes(float_byte_array, E);
	   Flash::write((uint32_t *)float_byte_array, addr + offset1,(uint32_t) 1);
	   float2Bytes(float_byte_array, SQUARE_ROOT_OF_TWO);
	   Flash::write((uint32_t *)float_byte_array, addr + offset2,(uint32_t) 1);

	   Flash::read(addr, (uint32_t *)float_byte_array, 1);
	   float first_result = Bytes2float(float_byte_array);
	   bool first_write = first_result * 10000 == PI * 10000;
	   Flash::read(addr + offset1, (uint32_t *)float_byte_array, 1);
	   float second_result = Bytes2float(float_byte_array);
	   bool second_write = second_result * 100000 == E * 100000;
	   Flash::read(addr + offset2, (uint32_t *)float_byte_array, 1);
	   float third_result = Bytes2float(float_byte_array);
	   bool third_write = third_result * 100000 == SQUARE_ROOT_OF_TWO * 100000;

	   return first_write && second_write && third_write;
	}

   bool test4_writing_in_sector_border(){
	   uint32_t data[] = {1024, 2048, 4096, 8192};
	   uint32_t data_result[4];
	   uint32_t addr = FLASH_SECTOR5_START_ADDRESS - 0x08;

	   Flash::write(&data[0], addr,(uint32_t) 4);
	   Flash::read(addr, &data_result[0], 4);

	   for(int i = 0; i < 4; i++){
		  if (data[i] != data_result[i]) {
			return false;
		}
	   }

	   return true;
   }
}
