#pragma once

#include "../ControlBlock.hpp"
#include "Integrator.hpp"

template <IntegratorType IntegratorMethod, typename T> class PI : public ControlBlock<T, T> {
    static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

public:
    T kp;
    T error;
    Integrator<IntegratorMethod> integrator;

public:
    PI() = default;
    PI(T kp, T ki, T period) : kp(kp), integrator(period, ki) {}
    void execute() override {
        integrator.input(this->input_value);
        integrator.execute();
        error = this->input_value;
        this->output_value = kp * error + integrator.output_value;
    }
    void set_kp(T kp) { this->kp = kp; }
    void set_ki(T ki) {
        integrator.ki = ki;
        integrator.reset();
    }
    void reset() { integrator.reset(); }
};
