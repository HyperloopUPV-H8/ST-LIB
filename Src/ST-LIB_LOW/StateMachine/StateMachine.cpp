/*
 * Created by Alejandro
 */

#include "StateMachine/StateMachine.hpp"

TimedAction::TimedAction(function<void()> action, uint32_t microseconds) :
	action(action), microseconds(microseconds) {}

void State::enter() {
	for (function<void()> action : on_enter_actions) {
		action();
	}
}

void State::exit() {
	for (function<void()> action : on_exit_actions) {
		action();
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

	for (uint8_t timed_action : current_state_timed_actions_in_microseconds) {
		Time::unregister_high_precision_alarm(timed_action);
	}
	for (uint8_t timed_action : current_state_timed_actions_in_milliseconds) {
		Time::unregister_low_precision_alarm(timed_action);
	}
	states[current_state].exit();

	current_state = new_state;

	states[current_state].enter();
	for (TimedAction timed_action : states[current_state].cyclic_actions) {
		if (timed_action.microseconds % 1000 == 0) {
			optional<uint8_t> optional_id = Time::register_low_precision_alarm(timed_action.microseconds/1000, timed_action.action);
			if (not optional_id) {
				//TODO: Error Handler
			}
			current_state_timed_actions_in_milliseconds.push_back(optional_id.value());

		} else {
			optional<uint8_t> optional_id = Time::register_high_precision_alarm(timed_action.microseconds, timed_action.action);
			if (not optional_id) {
				//TODO: Error Handler
			}
			current_state_timed_actions_in_microseconds.push_back(optional_id.value());
		}
	}
}
