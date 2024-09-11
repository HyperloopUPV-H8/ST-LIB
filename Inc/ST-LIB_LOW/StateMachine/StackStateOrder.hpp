#pragma once
#include "StateMachine/StateOrder.hpp"

template<size_t BufferLength,class... Types> requires NotCallablePack<Types*...>
class StackStateOrder : public StackOrder<BufferLength,Types...>{
public:
	StateMachine& state_machine;
	StateMachine::state_id state;
    StackStateOrder(uint16_t id,void(*callback)(void), StateMachine& state_machine, StateMachine::state_id state,Types*... values) : StackOrder<BufferLength,Types...>(id,callback,values...),
    		state_machine(state_machine), state(state) {
    	if(not state_machine.get_states().contains(state)){
    		ErrorHandler("State Machine does not contain state, cannot add StateOrder");
    		return;
    	}
    	else state_machine.get_states()[state].add_state_order(id);
    	Order::orders[id] = this;
    }

    void process() override {
    	if (this->callback != nullptr && state_machine.is_on && state_machine.current_state == state) this->callback();
    }

    void parse(OrderProtocol* socket, void* data)override{
    	if(state_machine.is_on && state_machine.current_state == state) StackOrder<BufferLength,Types...>::parse(data);
    }
};

#if __cpp_deduction_guides >= 201606
template<class... Types> requires NotCallablePack<Types*...>
StackStateOrder(uint16_t id,void(*callback)(void), StateMachine& state_machine, StateMachine::state_id state,Types*... values)->StackStateOrder<(!has_container<Types...>::value)*total_sizeof<Types...>::value, Types...>;
#endif