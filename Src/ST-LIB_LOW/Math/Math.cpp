#include "ST-LIB_LOW/Math/Math.hpp"
#include "CORDIC/CORDIC.hpp"

int32_t Math::array[4] = {0,0,0,0};
int32_t *Math::pointer1 = array;
int32_t *Math::pointer2 = (int32_t *) array + 1;
int32_t *Math::pointer3 = (int32_t *) array + 2;
int32_t *Math::pointer4 = (int32_t *) array + 3;

int32_t Math::sin(int32_t angle){
	*pointer1 = angle;
	RotationComputer::sin(pointer1,pointer2,1);
	return *pointer2;
}

int32_t Math::cos(int32_t angle){
	*pointer1 = angle;
	RotationComputer::cos(pointer1,pointer2,1);
	return *pointer2;
}

int32_t Math::tg(int32_t angle){
	if((angle & Q31_MASK) >= HALF_MARGIN){
		if((angle & Q31_MASK) >= SHALF_MARGIN){
			angle = angle - MAX_INT;
		}else{
			if(angle > 0){
				return MAX_INT;
			}else{
				return -MAX_INT-1;
			}
		}
	}
	*pointer1 = angle;
	RotationComputer::cos_and_sin(pointer1,pointer2,pointer3,1);
	return *pointer3 / (*pointer2 >> TG_DECIMAL_BITS);
}

int32_t Math::phase(int32_t x, int32_t y){
	*pointer1 = x;
	*pointer2 = y;
	RotationComputer::phase(pointer1, pointer2, pointer3, 1);
	return *pointer3;
}

uint32_t Math::modulus(int32_t x, int32_t y){
	if(((uint32_t) x + y) > MAX_MOD_MARGIN){
		*pointer1 = (x >> 1);
		*pointer2 = (y >> 1);
		RotationComputer::modulus(pointer1, pointer2, pointer3, 1);
		uint32_t mod= *pointer3;
		mod = (mod << 1);
		return mod;
	}
	*pointer1 = x;
	*pointer2 = y;
	RotationComputer::modulus(pointer1, pointer2, pointer3, 1);
	return *pointer3;
}

int32_t Math::atg(int32_t in){
	*pointer1 = 1 << TG_DECIMAL_BITS;
	*pointer2 = in;
	RotationComputer::phase(pointer1, pointer2, pointer3, 1);
	return *pointer3;
}


int32_t Math::sqrt(int32_t sq_in){
	int32_t sol = 0;
	int32_t temp = 0;
	int32_t over = sq_in >> SQ_DECIMAL_BITS;
	for(int32_t i = 0x8000 >> (SQ_DECIMAL_BITS >> 1); i >= 1; i = i >> 1){
		temp = (sol + i);
		if(((temp*temp)) <= over){
			sol = sol + i;
		}
	}

	return sol<<SQ_DECIMAL_BITS;
}

int32_t Math::sq_to_unitary(int32_t sq_in){
	return sq_in << (32 - SQ_DECIMAL_BITS);
}

int32_t Math::unitary_to_sq(int32_t in){
	return in >> (32 - SQ_DECIMAL_BITS);
}

int32_t Math::tg_to_unitary(int32_t tg_in){
	return tg_in << (32 - TG_DECIMAL_BITS);
}

int32_t Math::unitary_to_tg(int32_t in){
	return in >> (32 - TG_DECIMAL_BITS);
}

