/*
 * CORDIC.hpp
 *
 * Created on: 29 nov. 2022
 * 		Author: Ricardo
 */

#pragma once

#include "PinModel/Pin.hpp"
//#ifdef HAL_CORDIC_MODULE_ENABLED

	enum Operation_Computation{
		COSINE,
		SINE,
		PHASE,
		MODULUS,
		ARCTANGENT,
		SINE_COSINE,
		NONE

	};

class RotationComputer{
public:
	static void start();
	static void configurate(Operation_Computation Operation_mode);
	static void cos(int32_t *angle,int32_t *out,int size);
	static void sin(int32_t *angle,int32_t *out,int size);
	static void cos_and_sin(int32_t *angle, int32_t *cos_out, int32_t *sin_out, int size);
	static void phase(int32_t *x, int32_t *y, int32_t *angle_out, int size);
	static Operation_Computation mode;
	static float q31_to_f32(int32_t in);
	static int32_t f32_to_q31(float in);
};

//#endif
