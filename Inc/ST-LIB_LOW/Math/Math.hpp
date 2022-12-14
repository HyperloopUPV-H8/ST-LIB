/*
 * CORDIC.hpp
 *
 * Created on: 13 dic. 2022
 * 		Author: Ricardo
 */

#pragma once
#include "ST-LIB.hpp"
#define TG_DECIMAL_BITS 16
#define MAX_INT 2147483647
#define HALF_INT MAX_INT/2
#define MAX_MOD_MARGIN 2147483392
#define MAX_MARGIN 2147483392
#define HALF_MARGIN MAX_MARGIN/2
#define SHALF_MARGIN INT_MAX - HALF_MARGIN
#define Q31_MASK 0x7FFFFFFF


class Math{
public:
	static int32_t sin(int32_t angle);
	static int32_t cos(int32_t angle);
	static int32_t tg(int32_t angle);
	static int32_t phase(int32_t x, int32_t y);
	static int32_t modulus(int32_t x, int32_t y);
	static int32_t atg(int32_t tg_in);
	static inline int32_t tg_to_unitary(int32_t tg_in);
	static inline int32_t unitary_to_tg(int32_t in);
private:
	static int32_t array[4];
	static int32_t *pointer1;
	static int32_t *pointer2;
	static int32_t *pointer3;
	static int32_t *pointer4;
};
