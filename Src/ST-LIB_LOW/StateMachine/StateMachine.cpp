/*
 * Created by Alejandro
 */

#include <StateMachine/StateMachine.hpp>

void State::update() {
	for (function<void()> action : on_update_actions) {
		if (action) {
			action();
		}
	}
}

void State::enter() {
	for (function<void()> action : on_enter_actions) {
		if (action) {
			action();
		}
	}
}

void State::exit() {
	for (function<void()> action : on_exit_actions) {
		if (action) {
			action();
		}
	}
}

StateMachine::StateMachine(uint8_t initial_state) :
	initial_state(initial_state), current_state(initial_state) {
	states[initial_state] = State();
}

void StateMachine::add_state(uint8_t state) {
	if (states.contains(state)) {
		return; //TODO: Error handler
	}

	if (states.empty()) {
		initial_state = state;
		current_state = state;
	}
	states[state] = State();
}

void StateMachine::add_update_action(function<void()> action) {
	if (not current_state) {
		return; //TODO: Error handler
	}
	states[current_state].on_update_actions.push_back(action);
}
void StateMachine::add_update_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		return; //TODO: Error handler
	}
	states[state].on_update_actions.push_back(action);
}

void StateMachine::add_enter_action(function<void()> action) {
	if (not current_state) {
		return; //TODO: Error handler
	}
	states[current_state].on_enter_actions.push_back(action);
}
void StateMachine::add_enter_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		return; //TODO: Error handler
	}
	states[state].on_enter_actions.push_back(action);
}

void StateMachine::add_exit_action(function<void()> action) {
	if (not current_state) {
		return; //TODO: Error handler
	}
	states[current_state].on_exit_actions.push_back(action);
}
void StateMachine::add_exit_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		return; //TODO: Error handler
	}
	states[state].on_exit_actions.push_back(action);
}

void StateMachine::add_transition(uint8_t old_state, uint8_t new_state,
		function<bool()> transition) {
	if (not states.contains(old_state) or not states.contains(new_state)) {
		return; //TODO: Error handler
	}
	transitions[old_state][new_state] = transition;
}

void StateMachine::update() {
	states[current_state].update();
	check_transitions();
}

void StateMachine::check_transitions() {
	for (auto const state_transition : transitions[current_state]) {
		if (state_transition.second()) {
			force_change_state(state_transition.first);
		}
	}
}

void StateMachine::force_change_state(uint8_t new_state) {
	if (not states.contains(new_state)) {
		return; //TODO: Error handler
	}
	states[current_state].exit();
	current_state = new_state;
	states[current_state].enter();
}
