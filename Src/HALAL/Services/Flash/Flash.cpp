/*
 * Flash.cpp
 *
 *  Created on: Oct 19, 2022
 *      Author: Pablo
 */

#include "../Inc/HALAL/Services/Flash/Flash.hpp"



void Flash::read(uint32_t sourceAddr, uint32_t* result, uint32_t numberOfWords){
	if (sourceAddr < FLASH_START_ADDRESS || sourceAddr > FLASH_END_ADDRESS) {
			//TODO: Handle exception (memory out of limits)
			return;

	}

	HAL_FLASH_Unlock();
	while(1){
		*result = *(__IO uint32_t *)sourceAddr;
		sourceAddr += 4;
		result++;

		if (!(numberOfWords--)) {
			break;
		}
	}
	HAL_FLASH_Lock();
}

bool Flash::write(uint32_t * source, uint32_t dest_addr, uint32_t number_of_words){
	if (dest_addr < FLASH_SECTOR4_START_ADDRESS || dest_addr > FLASH_END_ADDRESS) {
		//TODO: Handle exception (memory out of limits)
		return false;
	}

	uint32_t start_relative_position_in_words;
	uint32_t end_relative_position_in_words;
	uint32_t buff_pos, source_pos;
	uint32_t index = 0;
	uint32_t buffer[SECTOR_SIZE_IN_WORDS];

	uint32_t start_sector = Flash::getSector(dest_addr);
	uint32_t start_sector_addr = Flash::getSectorStartingAddress(start_sector);
	uint32_t end_sector_address = dest_addr + ((number_of_words * 4) - 4);
	uint32_t end_sector = Flash::getSector(end_sector_address);

	Flash::read(start_sector_addr, buffer, SECTOR_SIZE_IN_WORDS);

	start_relative_position_in_words = (dest_addr - start_sector_addr) / 4 ;
	end_relative_position_in_words = start_relative_position_in_words + number_of_words - 1;
	source_pos = 0;
	for (buff_pos = start_relative_position_in_words; buff_pos <= end_relative_position_in_words && source_pos < number_of_words; ++buff_pos ) {
		buffer[buff_pos] = source[source_pos];
		source_pos++;
	}

	if (!Flash::erase(start_sector, end_sector)) {
		return false;
	}

	HAL_FLASH_Unlock();
	while(index < number_of_words){
		//Escribir parcialmente con lo nuevo mas lo viejo
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, start_sector_addr, (uint32_t)&buffer[index]) == HAL_OK) {
			start_sector_addr += 4 * FLASHWORD;
			index += FLASHWORD;
		}else{
			//TODO: Exception handle
			//TODO: Borrar la siguiente linea antes del pr
			auto aux = HAL_FLASH_GetError();
			HAL_FLASH_Lock();
			return false;
		}
	}
	//TODO: Notificar del exito
	HAL_FLASH_Lock();
	return true;
}

bool Flash::erase(uint32_t startSector, uint32_t endSector){
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t sectorError;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = startSector;
	EraseInitStruct.Banks    	  = FLASH_BANK_1;
	EraseInitStruct.NbSectors     = (endSector - startSector) + 1;

	HAL_FLASH_Unlock();

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &sectorError) != HAL_OK)
	{
		//TODO: Handle the exception
		//TODO: Borrar esta linea antes del pr
		auto aux = HAL_FLASH_GetError();
		HAL_FLASH_Lock();
		return false;
	}

	HAL_FLASH_Lock();
	return true;
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

