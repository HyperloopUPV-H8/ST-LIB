#include "../ControlBlock.hpp"
#pragma once

template <typename T, typename U = T>
concept Comparable = requires(T a, U b){
    { a < b };
    { b > a };
};

template <Comparable ComparableType>
class Saturator: public ControlBlock<ComparableType,ComparableType> {
    public:
        ComparableType upper_bound = 0, lower_bound = 0;
        Saturator(ComparableType default_input,ComparableType lower_bound,ComparableType upper_bound): ControlBlock<ComparableType,ComparableType>(default_input),
        lower_bound(lower_bound), upper_bound(upper_bound){}
        void execute() override {
            if(this->input_value > upper_bound){
                this->output_value = upper_bound;
            } else if(this->input_value < lower_bound){
                this->output_value = lower_bound;
            } else {
                this->output_value = this->input_value;
            }
        }
};