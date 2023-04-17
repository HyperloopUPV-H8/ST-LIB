//#include "../ControlSystem.hpp"
//#include "Integrator.hpp"
//#include "Derivator.hpp"
//
//#pragma once
//
//template<size_t N>
//class PID: public ControlBlock<double,double>{
//    public:
//        static constexpr size_t DerivationOrder = N-1;
//        double kp, kd;
//        double error, integral, derivative;
//        double previous_error;
//        BDFDerivator<N> derivator;
//        Integrator integrator;
//    public:
//        PID(double kp, double ki, double kd, double (&ks)[DerivationOrder], double period): kp(kp), kd(kd), derivator(period,ks), integrator(period,ki) {}
//        PID(double kp, double ki, double kd, double (&&ks)[DerivationOrder], double period): kp(kp), kd(kd), derivator(period,move(ks)), integrator(period,ki) {}
//        void execute() override {
//            derivator.input(this->input_value);
//            integrator.input(this->input_value);
//            derivator.execute();
//            integrator.execute();
//            previous_error = error;
//            error = this -> input_value;
//            this -> output_value = kp*error + integrator.output_value + kd*derivator.output_value;
//        }
//        void set_kp(double kp){
//            this->kp = kp;
//        }
//        void set_ki(double ki){
//            this->ki = ki;
//        }
//        void set_kd(double kd){
//            this->kd = kd;
//        }
//        void reset(){
//            integrator.reset();
//        }
//};
