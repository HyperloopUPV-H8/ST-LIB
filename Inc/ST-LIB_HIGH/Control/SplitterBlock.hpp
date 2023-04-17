#pragma once

#include "FeedbackControlBlock.hpp"

template<class InputType>
class SplitterBlock : public ControlBlock<InputType,InputType> {
public:
    using Input = InputType;
    using Output = InputType;
    function<void(InputType)> feedback_input;
    FeedbackControlBlock() = default;
    FeedbackControlBlock(InputType default_input): ControlBlock<InputType,InputType>(default_input){}
    void input (InputType input) override {
        this->input_value = input;
    }
    void execute() override {
        this->output_value = this->input_value;
        feedback();
    }
    void feedback(){
        if(feedback_input != nullptr){
            feedback_input(this->output_value);
        }
    }
    template<class AdditionalType>
    void connect(FeedbackControlBlock<OutputType,AdditionalType>& next){
        feedback_input = bind(&ControlBlock<OutputType,AdditionalType>::input_b, &next, placeholders::_1);
    }
};