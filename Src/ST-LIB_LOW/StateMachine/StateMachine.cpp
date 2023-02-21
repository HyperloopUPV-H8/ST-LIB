/*
 * Created by Alejandro
 */

#include "StateMachine/StateMachine.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

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
		ErrorHandler("The state %d is already added", state);
		return;
	}

	if (states.empty()) {
		initial_state = state;
		current_state = state;
	}
	states[state] = State();
}

void StateMachine::add_enter_action(function<void()> action) {
	if (not current_state) {
		ErrorHandler("The state machine does not have a current state");
		return;
	}
	states[current_state].on_enter_actions.push_back(action);
}

void StateMachine::add_enter_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}
	states[state].on_enter_actions.push_back(action);
}

void StateMachine::add_exit_action(function<void()> action) {
	if (not current_state) {
		ErrorHandler("The state machine does not have a current state");
		return;
	}
	states[current_state].on_exit_actions.push_back(action);
}
void StateMachine::add_exit_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine");
		return;
	}
	states[state].on_exit_actions.push_back(action);
}

void StateMachine::add_transition(uint8_t old_state, uint8_t new_state,
		function<bool()> transition) {
	if (not states.contains(old_state)) {
		ErrorHandler("The state %d is not added to the state machine", old_state);
	}

	if(not states.contains(new_state)) {
		ErrorHandler("The state %d is not added to the state machine", new_state);
		return;
	}

	transitions[old_state][new_state] = transition;
}

void StateMachine::add_state_machine(StateMachine* state_machine, uint8_t state) {
	nested_state_machine[state] = state_machine;

	if (current_state != state) {
		for (uint8_t timed_action : nested_sm->current_state_timed_actions_in_microseconds) {
			Time::unregister_high_precision_alarm(timed_action);
		}
		nested_sm->current_state_timed_actions_in_microseconds.clear();

		for (uint8_t timed_action : nested_sm->current_state_timed_actions_in_milliseconds) {
			Time::unregister_low_precision_alarm(timed_action);
		}
		nested_sm->current_state_timed_actions_in_milliseconds.clear();
	}
}

void StateMachine::check_transitions() {
	for (auto const state_transition : transitions[current_state]) {
		if (state_transition.second()) {
			force_change_state(state_transition.first);
		}
	}

	if (nested_state_machine.contains(current_state)) {
		nested_state_machine[current_state]->check_transitions();
	}
}

void StateMachine::force_change_state(uint8_t new_state) {
	if (not states.contains(new_state)) {
		ErrorHandler("The state %d is not added to the state machine", new_state);
		return;
	}

	for (uint8_t timed_action : current_state_timed_actions_in_microseconds) {
		Time::unregister_high_precision_alarm(timed_action);
	}
	current_state_timed_actions_in_microseconds.clear();

	for (uint8_t timed_action : current_state_timed_actions_in_milliseconds) {
		Time::unregister_low_precision_alarm(timed_action);
	}
	current_state_timed_actions_in_milliseconds.clear();

	if (nested_state_machine.contains(current_state)) {
		StateMachine* nested_sm = nested_state_machine[current_state];
		for (uint8_t timed_action : nested_sm->current_state_timed_actions_in_microseconds) {
			Time::unregister_high_precision_alarm(timed_action);
		}
		nested_sm->current_state_timed_actions_in_microseconds.clear();

		for (uint8_t timed_action : nested_sm->current_state_timed_actions_in_milliseconds) {
			Time::unregister_low_precision_alarm(timed_action);
		}
		nested_sm->current_state_timed_actions_in_milliseconds.clear();
	}

	states[current_state].exit();

	current_state = new_state;

	states[current_state].enter();

	if (nested_state_machine.contains(current_state)) {
		StateMachine* nested_sm = nested_state_machine[current_state];
		nested_sm->current_state = nested_sm->initial_state;
	}

	for (TimedAction timed_action : states[current_state].cyclic_actions) {
		if (timed_action.microseconds % 1000 == 0) {
			optional<uint8_t> optional_id = Time::register_low_precision_alarm(timed_action.microseconds/1000, timed_action.action);
			if (not optional_id) {
				ErrorHandler("The timed action could not be registered");
			}
			current_state_timed_actions_in_milliseconds.push_back(optional_id.value());

		} else {
			optional<uint8_t> optional_id = Time::register_high_precision_alarm(timed_action.microseconds, timed_action.action);
			if (not optional_id) {
				ErrorHandler("The timed action could not be registered");
			}
			current_state_timed_actions_in_microseconds.push_back(optional_id.value());
		}
	}
}
