#pragma once

#include "stdio.h"
#include "../ControlSystem.hpp"

template<size_t N>
class MovingAverage : public ControlBlock<double,double> {
    private:
		double buffer[N] = {0.0};
        uint32_t first = 0, last = -1;
        uint32_t counter = 0;
        double accumulator = 0.0;

    public:
        MovingAverage(): ControlBlock<double,double>(){output_value = 0.0;}

        void execute() override {
        	if(counter < N) {
                last++;
        		buffer[last] = input_value;
        		accumulator += input_value/N;
        		counter++;
        		return;
        	}
        	accumulator -= buffer[first] / N;
            first = (first + 1) % N;
            last = (last + 1) % N;
            buffer[last] = input_value;
            accumulator += buffer[last] / N;
            output_value = accumulator;
            return;
        }

        double compute(double input_v) {
        	this->input(input_v);
        	execute();
        	return this->output_value;
        }

        void reset(){
        	first = 0;
        	last = -1;
        	counter = 0;
        	accumulator = 0;
        	output_value = 0;
        	for(double& e: buffer){
				e = 0;
        	}
        }
};


template<class InputType, class OutputType, size_t N>
class MovingAverageBlock : public ControlBlock<InputType,OutputType> {
    private:
		InputType buffer[N] = {(InputType)0};
        uint32_t first = 0, last = -1;
        uint32_t counter = 0;
        InputType accumulator = 0.0;

    public:
        MovingAverageBlock(): ControlBlock<InputType,OutputType>(){this->output_value = 0;}

        void execute() override {
        	if(counter < N) {
                last++;
        		buffer[last] = this->input_value;
        		accumulator += this->input_value/N;
        		counter++;
        		return;
        	}
        	accumulator -= buffer[first] / N;
            first = (first + 1) % N;
            last = (last + 1) % N;
            buffer[last] = this->input_value;
            accumulator += buffer[last] / N;
            this->output_value = (OutputType) accumulator;
            return;
        }

        double compute(InputType input_v) {
        	this->input(input_v);
        	execute();
        	return this->output_value;
        }

        void reset(){
        	first = 0;
        	last = -1;
        	counter = 0;
        	accumulator = 0;
        	this->output_value = 0;
        	for(InputType& e: buffer){
				e = 0;
        	}
        }
};

