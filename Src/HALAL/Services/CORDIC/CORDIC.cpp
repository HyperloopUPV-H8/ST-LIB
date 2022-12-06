/*
 * CORDIC.cpp
 *
 * Created on: 29 nov. 2022
 * 		Author: Ricardo
 */

#include "CORDIC/CORDIC.hpp"


Operation_Computation RotationComputer::mode = NONE;



void RotationComputer::cos(int32_t *in, int32_t *out, int size){
	if(RotationComputer::mode != COSINE){
		RotationComputer::mode = COSINE;
		MODIFY_REG(CORDIC -> CSR,
					0x007F07FF,
					0x00000050
		);
	}
	for(int n = 0; n < size; n++){
		CORDIC -> WDATA = in[n];
		out[n] = CORDIC -> RDATA;
	}
}

void RotationComputer::sin(int32_t *in, int32_t *out, int size){
	if(RotationComputer::mode != SINE){
		RotationComputer::mode = SINE;
		MODIFY_REG(CORDIC -> CSR,
					0x007F07FF,
					0x00000051
		);
	}
	for(int n = 0; n < size; n++){
		CORDIC -> WDATA = in[n];
		out[n] = CORDIC -> RDATA;
	}
}

void RotationComputer::cos_and_sin(int32_t *in, int32_t *cos_out, int32_t *sin_out, int size){
	if(RotationComputer::mode != SINE_COSINE){
		RotationComputer::mode = SINE_COSINE;
		MODIFY_REG(CORDIC -> CSR,
					0x007F07FF,
					0x00100050
		);
	}
	for(int n = 0; n < size; n++){
		CORDIC -> WDATA = in[n];
		cos_out[n] = READ_REG(CORDIC -> RDATA);
		sin_out[n] = READ_REG(CORDIC -> RDATA);
	}
}



void RotationComputer::configurate(Operation_Computation operation_mode) {

	/*In CSR you can directly configurate CORDIC. The masks for the variables are defined as follow
	 Functions : 0x0000000F (0.COSINE 1.SINE 2.PHASE 3.MODULUS 4.ARCTANGENT 5.HCOSINE 6.HSINE 7.HARCTANGENT 8.NATLOG 9.SQRT)
	 Precision : 0x000000F0 (0.1_cycle 1.2_cycles 2.3_cycles ...)
	 Scale : 0x00000700 (0 = 0, 1 = 1 ... means that the in has been shifted to the right and the output needs to be shifted to the left)
	 Interrupt enabled : 0x00010000 (0.disabled 1.enabled)
	 DMA read and write : 0x00020000 0x00040000 (respectively)
	 In and out arguments: 0x00080000 0x00100000 (respectively) (0.1argument 1.2arguments)
	 Width input and output: 0x00200000 0x00400000 (respectively) (0.32bytes 1.16bytes)
	 Ready flag : 0x80000000

	 */
	switch(operation_mode){
	case COSINE:
			MODIFY_REG(CORDIC -> CSR,//REGISTER
				0x007F07FF, //CLEAR MASK
				0x00000050	//WRITE
				);
			RotationComputer::mode = COSINE;
			break;
	case SINE:
		MODIFY_REG(CORDIC -> CSR,
			0x007F07FF,
			0x00000051
			);
		RotationComputer::mode = SINE;
		break;
	case SINE_COSINE:
			MODIFY_REG(CORDIC -> CSR,
				0x007F07FF,
				0x00000051
				);
			RotationComputer::mode = SINE_COSINE;
			break;
	}


}


float RotationComputer::q31_to_f32(int32_t in){
	return ldexp((int32_t)in, -31);
}

int32_t RotationComputer::f32_to_q31(float in){
	const float MAX_F = 0.99995f;
	const float MIN_F = -0.99995f;
	return (int) roundf(scalbnf(in/M_PI,31));
}

