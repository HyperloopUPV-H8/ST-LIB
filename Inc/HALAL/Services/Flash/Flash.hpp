/*
 * Flash.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: Pablo
 */

#ifndef HALAL_SERVICES_FLASH_FLASH_HPP_
#define HALAL_SERVICES_FLASH_FLASH_HPP_

#include "stm32h7xx_hal.h"
#include "../../../C++Utilities/CppUtils.hpp"

#define FLASHWORD 8
#define FLASH_START_ADDRESS 0x08000000
#define FLASH_SECTOR0_START_ADDRESS 0x08000000
#define FLASH_SECTOR1_START_ADDRESS 0x08020000
#define FLASH_SECTOR2_START_ADDRESS 0x08040000
#define FLASH_SECTOR3_START_ADDRESS 0x08060000
#define FLASH_SECTOR4_START_ADDRESS 0x08080000
#define FLASH_SECTOR5_START_ADDRESS 0x080A0000
#define FLASH_SECTOR6_START_ADDRESS 0x080C0000
#define FLASH_SECTOR7_START_ADDRESS 0x080E0000
#define FLASH_END_ADDRESS 0x080FFFFF
#define SECTOR_SIZE_IN_WORDS 32767

/*
 * Example of usage if we want to read a float from flash:
 *
 * 	uint8_t buffer[4];
 *	float value;
 *
 *	read(sourceAddr, (uint32_t *)buffer, 1);
 *	value = Bytes2float(buffer);
 *	return value;
 *
 */

class Flash{

public:
	static void read(uint32_t sourceAddr, uint32_t* result, uint32_t numberOfWords);

	static bool write(uint32_t* source, uint32_t destAddr, uint32_t sizeInWords);

	static bool erase(uint32_t initAddr, uint32_t finalAddr); //TODO: Este metodo tiene que ser privado antes de un pull request

private:

	static uint32_t getSector(uint32_t Address);

	static uint32_t getSectorStartingAddress(uint32_t sector);



};



#endif /* HALAL_SERVICES_FLASH_FLASH_HPP_ */
