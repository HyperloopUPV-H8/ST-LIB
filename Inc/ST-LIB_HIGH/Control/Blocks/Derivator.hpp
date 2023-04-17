#pragma once

#include "../ControlBlock.hpp"
template<int N>
class BDFDerivator : public ControlBlock<double,double>{
    public:
        double period;
        static constexpr int order = N-1;
        double buffer[N+1] = {0.0};
        double (&ks)[order];
        int index = 0;
        uint64_t counter = 0;
    public:
        BDFDerivator(double period, double(&ks)[order]): period(period), ks(ks){this->output_value = 0.0;}
        BDFDerivator(double period, double(&&ks)[order]): period(period), ks(move(ks)){this->output_value = 0.0;}
        void execute()override{
            if(counter++<order) {
                buffer[index] = input_value;
                index++;
                index %= (N+1);
                output_value = 0.0;
                return;
            }
            buffer[index] = input_value;
            output_value = 0.0;
            for(int i = 1; i <= order; i++){
                output_value += ks[i-1]*(buffer[index]-buffer[((index-i)%(N+1) + (N+1))%(N+1)])/i;
            }
            output_value /= (period*order);
            index++;
            index %= (N+1);
            counter--;
        }
};

class SimpleDerivator: public ControlBlock<double,double>{
    public:
        double period;
        static constexpr int N = 2;
        double k = 1;
        double buffer[N] = {0.0};
        int index = 0;
    public:
        SimpleDerivator(double period): period(period){this->output_value = 0.0;}
        SimpleDerivator(double period, double k): period(period), k(k){this->output_value = 0.0;}
        void execute(){
            buffer[index] = input_value;
            output_value = k*buffer[((index-1)%(N+1) + (N+1))%(N+1)]/(period*buffer[index]);
            index++;
            index %= N;
            return;
        }
};