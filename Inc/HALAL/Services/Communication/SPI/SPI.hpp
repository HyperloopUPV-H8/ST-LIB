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
		bool tx_status;
		queue<SPIPacket> tx_queue;
		uint8_t* rx_temporal_buffer;
		uint16_t rx_temporal_buffer_size;
		bool rx_status;
		queue<SPIPacket> rx_queue;

		bool operator<(const SPI::Peripheral& other) const {
			return SS < other.SS;
		}

		bool operator==(const SPI::Peripheral& spi) const
		{
			return SCK == spi.SCK && MOSI == spi.MOSI && MISO == spi.MISO && SS == spi.SS;
		}

	public:
		void set_rx_status(bool new_status);

		void set_tx_status(bool new_status);
	};

	struct SPIPeripheral_hash_function {
		size_t operator()(const SPI::Peripheral& spi) const
		{
			return hash<uint32_t>()((uint32_t)spi.SS.pin);
		}
	};

private:
	static void try_receive_next_packet(uint8_t id, uint16_t data_size);

public:
	static forward_list<uint8_t> ID_manager;
	static unordered_map<uint8_t, SPI::Peripheral > registered_spi;

	SPI::Peripheral peripheral1 = {.SCK = PC10, .MOSI = PB2, .MISO = PC11, .SS = PA4,
								   .hspi = &hspi3,
								   .tx_status = true, .tx_queue = queue<SPIPacket>(),
								   .rx_temporal_buffer = nullptr, .rx_temporal_buffer_size = 0,
								   .rx_status = true, .rx_queue = queue<SPIPacket>()
								  };

	static optional<uint8_t> register_SPI(SPI::Peripheral& spi);

	static void send_next_packet(uint8_t id, SPIPacket& packet);

	//TODO: Use of this method is discouraged
	static void try_send_next_packet(uint8_t id);

	static optional<SPIPacket> get_next_packet(uint8_t id, uint16_t data_size);

};
