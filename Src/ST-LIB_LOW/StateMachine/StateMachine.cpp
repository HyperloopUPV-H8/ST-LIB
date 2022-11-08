/*
 * Created by Alejandro
 */

#include <StateMachine/StateMachine.hpp>

void StateMachine::add_update_action(Action* action) {
	current_state.actions.push_back(action);
}

void StateMachine::add_update_action(Action* action, State state) {
	state.actions.push_back(action);
}

void StateMachine::add_enter_action(State old_state, State new_state,
		Action* action) {
	on_enter[old_state][new_state].push_back(action);
}

void StateMachine::add_transition(State old_state, State new_state,
		Transition* transition) {
	transitions[old_state][new_state] = transition;
}

void StateMachine::update() {
	for (Action* action : on_update[current_state]) {
		if (action != nullptr) {
			(*action)();
		}
	}
}

void StateMachine::check_transitions() {
	for (auto const &mem : transitions[current_state]) {
		if ((*mem.second)()) {
			change_state(mem.first);
		}
	}
}

void StateMachine::change_state(State new_state) {
	for (auto const action: on_enter[current_state][new_state]) {
		if (action != nullptr) {
			(*action)();
		}
	}
	current_state = new_state;
}
