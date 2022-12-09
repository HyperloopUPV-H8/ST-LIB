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
		SINE_COSINE,
		PHASE,
		MODULUS,
		PHASE_MODULUS,
		ARCTANGENT,
		NONE

	};

//Angles go in the range of [-pi, pi] radians for ints and [0,2*pi] for uints
//Coordinates go in the range of [-1,1] for ints (they work on uints but not in a logical way)

class RotationComputer{
public:
	//a 180º angle is 2147483647.
	static void cos(int32_t *angle,int32_t *out,int size);
	static void sin(int32_t *angle,int32_t *out,int size);
	static void cos_and_sin(int32_t *angle, int32_t *cos_out, int32_t *sin_out, int size);
	//vector values can go from (-2147483647 to 2147483647)
	// vector (x,0) is phase 0
	static void phase(int32_t *x, int32_t *y, int32_t *angle_out, int size);
	//cannot give a value higher than 2147483392
	static void modulus(int32_t *x, int32_t *y, int32_t *out, int size);
	static void phase_and_modulus(int32_t *x, int32_t *y, int32_t *angle_out, int32_t *mod_out, int size);
	static Operation_Computation mode;
	static float q31_to_radian_f32(uint32_t in);
	static int32_t radian_f32_to_q31(double in);
};

//#endif
