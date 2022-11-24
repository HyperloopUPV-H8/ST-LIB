/*
 * DMAStream.hpp
 *
 *  Created on: Nov 23, 2022
 *      Author: alejandro
 */

#pragma once

struct DMAStream {
public:
	uint16_t* buffer;
	bool is_on = false;

	DMAStream() = default;
	DMAStream(uint16_t* buffer) : buffer(buffer), is_on(false) {};
};
