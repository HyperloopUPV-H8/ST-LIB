/*
 * CORDIC.hpp
 *
 * Created on: 13 dic. 2022
 * 		Author: Ricardo
 */

#pragma once
#include "ST-LIB.hpp"

class Math{
public:
	static uint32_t sin(uint32_t angle);
	static uint32_t cos(uint32_t angle);
	static uint32_t tg(uint32_t angle);
	static uint32_t phase(uint32_t x, uint32_t y);
	static uint32_t modulus(uint32_t x, uint32_t y);
private:
	uint32_t array [4];
	uint32_t *pointer1;
	uint32_t *pointer2;
	uint32_t *pointer3;
	uint32_t *pointer4;
};
