#pragma once
#include "../ControlBlock.hpp"

#pragma once

class Integrator : public ControlBlock<double,double>{
    public:
        static constexpr int N = 2;
        double period;
        double buffer[N] = {0.0};
        int index = 0;
        double integral = 0.0;
        double ki;
        bool first_execution = true;
    public:
        Integrator(double period, double ki): period(period), ki(ki) {this->output_value = 0.0;}
        void execute() override{
            buffer[index] = input_value;
            if(first_execution){
                first_execution = false;
                index++;
                index %= N;
                output_value = 0.0;
                return;
            } 
            integral += ki*period*(buffer[index] + buffer[((index-1)%(N) + (N))%(N)])/2.0;
            output_value = integral;
            index++;
            index %= N;
            return;
        }
        void reset(){
            integral = 0.0;
            index = 0;
            for(int i = 0; i < N; i++){
                buffer[i] = 0.0;
            }
        }

};
