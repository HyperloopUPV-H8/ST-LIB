#pragma once

#include "stdio.h"
#include "../ControlSystem.hpp"

double computeMA(double input_value);

template<size_t N>
class MovingAverage : public ControlBlock<double,double> {
    private:
		double buffer[N] = {0};
        uint32_t first = 0, last = 0;
        uint32_t counter = 0;

    public:
        MovingAverage(): ControlBlock<double,double>(){output_value = 0.0;}

        void execute() override {
        	while(counter < N){
        		if(counter == 0) buffer[first] = input_value;
        		buffer[last] = input_value;
        		last++;
        		output_value += input_value/N;
        		counter++;
        		return;
        	}
            output_value -= first / N;
            first = (first + 1) % N;

            last = (last + 1) % N;
            buffer[last] = input_value;
            output_value += buffer[last] / N;
        }

        double compute(double input_v) {
            input(input_v);
            execute();
            return output_value;
        }
};

