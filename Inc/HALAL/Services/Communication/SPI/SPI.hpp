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

extern SPI_HandleTypeDef hspi3;

class SPI{

private:
	struct Peripheral
	{
		Pin SCK;
		Pin MOSI;
		Pin MISO;
		Pin SS;
		SPI_HandleTypeDef* hspi;
		optional<SPIPacket> tx_buffer;
		bool receive_ready;

		bool operator<(const SPI::Peripheral& other) const {
			return SS < other.SS;
		}

		bool operator==(const SPI::Peripheral& spi) const
		{
			return SCK == spi.SCK && MOSI == spi.MOSI && MISO == spi.MISO && SS == spi.SS;
		}
	};

	struct SPIPeripheral_hash_function {
		size_t operator()(const SPI::Peripheral& spi) const
		{
			return hash<uint32_t>()((uint32_t)spi.SS.pin);
		}
	};

public:
	static forward_list<uint8_t> ID_manager;
	static unordered_map<uint8_t, SPI::Peripheral* > registered_spi;

	static SPI::Peripheral peripheral3;

	static optional<uint8_t> register_SPI(SPI::Peripheral& spi);

	static bool transmit_next_packet(uint8_t id, SPIPacket& packet);

	static bool receive_next_packet(uint8_t id, SPIPacket& packet);

	static bool has_next_packet(uint8_t id);

	static bool is_busy(uint8_t id);

};
