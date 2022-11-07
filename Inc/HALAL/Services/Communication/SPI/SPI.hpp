/*
 * SPI.hpp
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo
 */

#pragma once

#include "ST-LIB.hpp"

struct SPI_HandleTypeDef {
	string spi;
};
SPI_HandleTypeDef hspi3("spi3");

struct SPIPeripheral
{
	string spi_num;
	string SCK; //TODO: Change string to PIN
	string MOSI;
	string MISO;
	string SS;

	bool operator<(const SPIPeripheral& other) const {
		return spi_num < other.spi_num;
	}

	bool operator==(const SPIPeripheral& spi) const
	{
		return spi_num == spi.spi_num;
	}
};

struct SPIPeripheral_hash_function {
	size_t operator()(const SPIPeripheral& spi) const
	{
		return hash<string>()(spi.spi_num);
	}
};

extern SPIPeripheral SPI1("spi3", "PC10", "PC12", "PC11", "PD0");

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
