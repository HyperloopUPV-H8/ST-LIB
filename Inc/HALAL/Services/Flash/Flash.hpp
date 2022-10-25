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
	**IMPORTANTE**
	Hay que reservar la memoria en el linker script para
	evitar que el debugger modifique este espacio.
	Se hace de la siguiente manera:
	 MEMORY { 
		...... 
   		DATA (xrw) : ORIGIN = 0x8080000, LENGTH = 512K //Allocated half of the flash 
		......
		}

	Si queremos crear variables en la flash hay que crear una seccion.
	(En principio ninguno de los pasos de abajo es necesario para 
	escribir en la flash)
	*Leer nota al final*
	Se hace de la siguiente manera:
	 .user_data : 
 
     	{ . = ALIGN(4); 
 
       	*(.user_data)
 
       	. = ALIGN(4);
 
     } >DATA

	Declaramos la varibale de la siguiente manera:
	 attribute((section(".user_data"))) const uint8_t userConfig[64]
	
	Para escribir en esta variable hay que hacerlo usando la HAL.
	Se hace de la siguiente manera:
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)&userConfig[index], someData);

	Las variables en la flash hay que investigar si funcionan y como funionan
	lo dejo escrito porque la posibilidad existe y puede ser interesante
	tenerlas en cuenta.
*/


/*
 * Example of usage if we want to read a float from flash:
 *
 * 	uint8_t buffer[4];
 *	float value;
 *
 *	read(sourceAddr, (uint32_t *)buffer, 1);
 *	value = Bytes2float(buffer);
 *	
 */

class Flash{

public:
	static void read(uint32_t sourceAddr, uint32_t* result, uint32_t numberOfWords);

	static bool write(uint32_t* source, uint32_t destAddr, uint32_t sizeInWords);

	static bool erase(uint32_t startSector, uint32_t endSector); //TODO: Este metodo tiene que ser privado antes de un pull request

private:

	static uint32_t getSector(uint32_t Address);

	static uint32_t getSectorStartingAddress(uint32_t sector);



};



#endif /* HALAL_SERVICES_FLASH_FLASH_HPP_ */
