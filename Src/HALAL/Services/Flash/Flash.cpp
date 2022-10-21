/*
 * Flash.cpp
 *
 *  Created on: Oct 19, 2022
 *      Author: Pablo
 */

#include "Flash.hpp"

#define FLASHWORD 8
#define FLASH_START_ADDRESS 0x08000000
#define FLASH_END_ADDRESS 0x080FFFFF
#define SECTOR_SIZE_IN_WORDS 32000

void Flash::read(uint32_t sourceAddr, uint32_t* result, uint32_t numberOfWords){
	if (sourceAddr < FLASH_START_ADDRESS || sourceAddr > FLASH_END_ADDRESS) {
			//TODO: Handle exception (memory out of limits)
			return;
	}

	while(numberOfWords != 0){
		*result = *(__IO uint32_t *)sourceAddr;
		sourceAddr += 4;
		result++;
		numberOfWords--;
	}
}

void Flash::write(uint32_t* source, uint32_t destAddr, uint32_t numberOfWords){
	if (destAddr < FLASH_START_ADDRESS || destAddr > FLASH_END_ADDRESS) {
		//TODO: Handle exception (memory out of limits)
		return;
	}

	uint32_t startRelativePositionInWords;
	uint32_t endRelativePositionInWords;

	uint32_t index = 0;
	uint32_t buffer[SECTOR_SIZE_IN_WORDS];

	uint32_t startSector = getSector(destAddr);
	uint32_t sectorStartingAddress = getSectorStartingAddress(startSector);
	read(sectorStartingAddress, buffer, SECTOR_SIZE_IN_WORDS);

	HAL_FLASH_Unlock();

	//WARNING: Hay que testear muy bien esto
	startRelativePositionInWords = (destAddr - FLASH_START_ADDRESS) / 4;
	endRelativePositionInWords = startRelativePositionInWords + numberOfWords;
	uint32_t buffPos, sourcePos;
	sourcePos = 0;
	for (buffPos = startRelativePositionInWords; buffPos <= endRelativePositionInWords && sourcePos < numberOfWords; ++buffPos ) {
		buffer[buffPos] = source[sourcePos];
		sourcePos++;
	}

	Flash::erase(destAddr, (destAddr + numberOfWords * 4));

	while(index < numberOfWords){
		//Escribir parcialmente con lo nuevo mas lo viejo
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, destAddr, buffer[index]) == HAL_OK) {
			destAddr += 4 * FLASHWORD;
			index += FLASHWORD;
		}else{
			HAL_FLASH_Lock();
			//TODO: Exception handle
			return;
		}
	}
	//TODO: Notificar del exito
	HAL_FLASH_Lock();

}

void Flash::erase(uint32_t initAddr, uint32_t finalAddr){
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t sectorError;

	uint32_t startSector = Flash::getSector(initAddr);
	uint32_t endSectorAddress = finalAddr;
	uint32_t endSector = getSector(endSectorAddress);

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = startSector;
	EraseInitStruct.Banks    	  = FLASH_BANK_1;
	EraseInitStruct.NbSectors     = (endSector - startSector) + 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &sectorError) != HAL_OK)
	{
		//TODO: Handle the exception
		return;
	}
}

uint32_t Flash::getSector(uint32_t Address)
{
  uint32_t sector = 0;

  /* BANK 1 */
  if((Address >= 0x08000000) && (Address < 0x08020000))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address >= 0x08020000) && (Address < 0x08040000))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address >= 0x08040000) && (Address < 0x08060000))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address >= 0x08060000) && (Address < 0x08080000))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address >= 0x08080000) && (Address < 0x080A0000))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address >= 0x080A0000) && (Address < 0x080C0000))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address >= 0x080C0000) && (Address < 0x080E0000))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address >= 0x080E0000) && (Address < 0x08100000))
  {
    sector = FLASH_SECTOR_7;
  }

  return sector;
}

uint32_t Flash::getSectorStartingAddress(uint32_t sector){
	uint32_t address;
	switch (sector) {
		case FLASH_SECTOR_0:
			address = 0x08000000;
		break;
		case FLASH_SECTOR_1:
			address = 0x08020000;
		break;
		case FLASH_SECTOR_2:
			address = 0x08040000;
		break;
		case FLASH_SECTOR_3:
			address = 0x08060000;
		break;
		case FLASH_SECTOR_4:
			address = 0x08080000;
		break;
		case FLASH_SECTOR_5:
			address = 0x080A0000;
		break;
		case FLASH_SECTOR_6:
			address = 0x080C0000;
		break;
		case FLASH_SECTOR_7:
			address = 0x080E0000;
		break;
		default:
			address = 0x08000000;
			break;
	}

	return address;
}

