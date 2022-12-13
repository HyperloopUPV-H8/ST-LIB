/*
 * CORDIC.hpp
 *
 * Created on: 13 dic. 2022
 * 		Author: Ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#define TG_DECIMAL_BITS 16
#define INT_MAX 2147483647


class Math{
public:
	static int32_t sin(int32_t angle);
	static int32_t cos(int32_t angle);
	static int32_t tg(int32_t angle);
	static int32_t phase(int32_t x, int32_t y);
	static int32_t modulus(int32_t x, int32_t y);
private:
	static int32_t array[4];
	static int32_t *pointer1;
	static int32_t *pointer2;
	static int32_t *pointer3;
	static int32_t *pointer4;
};
