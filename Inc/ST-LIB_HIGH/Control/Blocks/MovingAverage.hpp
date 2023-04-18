#pragma once

#include "stdio.h"
#include "../ControlSystem.hpp"

double computeMA(double input_value);

template<size_t N>
class MovingAverage : public ControlBlock<double,double> {
    private:
        double buffer[N] = {0.0};
        int first = 0, last = 0;
    public:
        MovingAverage(): ControlBlock<double,double>(){output_value = 0.0;}

        void execute() override {
            output_value -= buffer[first] / N;
            first = (first + 1) % N;

            buffer[last] = input_value;
            output_value += buffer[last] / N;
            last = (last + 1) % N;

        }

        double compute(double input_v) {
            input(input_v);
            execute();
            return output_value;
        }
};

