/*
 * Flash.hpp
 *
 *  Created on: 19 oct. 2022
 *      Author: Pablo
 */

#pragma once
#include "ST-LIB-CORE.hpp"


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
#define SECTOR_SIZE_IN_WORDS 32768

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

	Más información sobre problemas y soluciones de usar la FLASH en la siguiente issue
	en github: https://github.com/HyperloopUPV-H8/ST-LIB/issues/16
*/

class Flash{

public:
	static void read(uint32_t source_addr, uint32_t* result, uint32_t number_of_words);

	static bool write(uint32_t* source, uint32_t dest_addr, uint32_t size_in_words);

	static bool erase(uint32_t start_sector, uint32_t end_sector);

private:

	static uint32_t get_sector(uint32_t address);

	static uint32_t get_sector_starting_address(uint32_t sector);
};
