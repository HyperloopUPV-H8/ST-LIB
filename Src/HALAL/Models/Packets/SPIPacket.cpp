/*
 * SpiPacket.hpp
 *
 *  Created on: 15 nov. 2023
 *      Author: ricardo
 */

#include "HALAL/Models/Packets/SPIPacket.hpp"

map<uint16_t, SPIPacket*> SPIPacket::SPIPacketsByID {};
