/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "ST-LIB.hpp"
#include "C++Utilities/CppUtils.hpp"
#include "Communication/SPI/SPIPacket.hpp"

struct SPIPeripheral
{
	Pin SCK; //TODO: Change string to PIN
	Pin MOSI;
	Pin MISO;
	Pin SS;

	bool operator<(const SPIPeripheral& other) const {
		return SS < other.SS;
	}

	bool operator==(const SPIPeripheral& spi) const
	{
		return SCK == spi.SCK && MOSI == spi.MOSI && MISO == spi.MISO && SS == spi.SS;
	}
};

extern SPIPeripheral SPIPeripheral1(PE0, PE0, PE0, PE0);

struct SPIPeripheral_hash_function {
	size_t operator()(const SPIPeripheral& spi) const
	{
		return hash<uint32_t>()((uint32_t)&spi.SS + (uint32_t)&spi.MOSI);
	}
};

class SPI
{

private:
	static forward_list<uint8_t> ID_manager;
	static unordered_map<SPIPeripheral, SPI_HandleTypeDef*, SPIPeripheral_hash_function> spi_un_map;

public:
	static unordered_map<uint8_t, SPI_HandleTypeDef*> registered_spi;

	static unordered_map<uint8_t, pair<bool, queue<SPIPacket>>> tx_packet_buffer;

public:

	static optional<uint8_t> register_SPI(SPIPeripheral& spi);

	static void send_packet_by_SPI(uint8_t id, SPIPacket& packet);
	static void try_send_next_packet(uint8_t id);
};
