/*
 * SpiOrder.hpp
 *
 *  Created on: 26 jan. 2023
 *      Author: ricardo
 */

#include "Packets/SPIOrder.hpp"

map<uint16_t, SPIBaseOrder*> SPIBaseOrder::SPIOrdersByID {};

SPIBaseOrder::~SPIBaseOrder(){};
