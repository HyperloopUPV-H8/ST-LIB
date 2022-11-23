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
	uint8_t buffer_length;
	bool is_on = false;

	DMAStream() = default;
	DMAStream(uint16_t* buffer, uint8_t buffer_length) : buffer(buffer), buffer_length(buffer_length), is_on(false) {};
};
