#include "Inc/ST-LIB_LOW/StateMachine/StateMachine.hpp"
#include <iostream>

template <auto V> struct constant_eval {};
template <typename F> concept CanCompile = requires { typename constant_eval<F::invoke()>; };

enum class MasterState { A, B, C };

struct SelfTransitionCheck {
    static consteval bool invoke() {
        auto stA = make_state(MasterState::A, Transition<MasterState>{MasterState::A, []{ return true; }});
        return true;
    }
};

struct InvalidActionStateCheck {
    static consteval bool invoke() {
        auto stA = make_state(MasterState::A);
        auto sm = make_state_machine(MasterState::A, stA);
        auto stB = make_state(MasterState::B);
        sm.add_enter_action([]{}, stB);
        return true;
    }
};

int main() {
    std::cout << "SelfTransitionCheck (should be 0): " << CanCompile<SelfTransitionCheck> << "\n";
    std::cout << "InvalidActionStateCheck (should be 0): " << CanCompile<InvalidActionStateCheck> << "\n";
    return 0;
}
