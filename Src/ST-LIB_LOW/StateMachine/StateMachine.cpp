/*
 * Created by Alejandro
 */

#include "StateMachine/StateMachine.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

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

/**
 * This is a constructor for a StateMachine object that initializes the initial state and creates a
 * State object for it.
 * 
 * @param initial_state The initial state parameter is an unsigned 8-bit integer that represents the
 * starting state of the state machine.
 */
StateMachine::StateMachine(uint8_t initial_state) :
	initial_state(initial_state), current_state(initial_state) {
	states[initial_state] = State();
}

/**
 * The function adds a state to a state machine and sets it as the initial and current state if it is
 * the first state added.
 * 
 * @param state The state to be added to the state machine.
 * 
 * @return The function does not return anything. It has a void return type.
 */
void StateMachine::add_state(uint8_t state) {
	if (states.contains(state)) {
		ErrorHandler("The state %d is already added", state);
		return;
	}

	if (states.empty()) {
		initial_state = state;
		current_state = state;
	}
	states[state];
}

/**
 * This function adds an action to be executed when entering the current state of a state machine.
 * 
 * @param action The parameter "action" is a function pointer to a function that takes no arguments and
 * returns nothing (void). It represents an action that should be executed when the current state of
 * the state machine is entered.
 */
void StateMachine::add_enter_action(function<void()> action) {
	if (not current_state) {
		ErrorHandler("The state machine does not have a current state");
		return;
	}
	states[current_state].on_enter_actions.push_back(action);
}

/**
 * This function adds an action to be executed when entering a specific state in a state machine.
 * 
 * @param action The action parameter is a function pointer to a function that takes no arguments and
 * returns nothing. This function will be executed when the state machine transitions to the state
 * specified by the state parameter.
 * @param state The state parameter is an unsigned 8-bit integer that represents the state to which the
 * enter action is being added.
 */
void StateMachine::add_enter_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}
	states[state].on_enter_actions.push_back(action);
}

/**
 * This function adds an exit action to the current state of a state machine.
 * 
 * @param action `action` is a function pointer to a function that takes no arguments and returns
 * nothing (`void`). It represents an action that should be executed when the state machine exits the
 * current state and transitions to a new state. The `add_exit_action` function adds this action to the
 * list of exit actions for
 */
void StateMachine::add_exit_action(function<void()> action) {
	if (not current_state) {
		ErrorHandler("The state machine does not have a current state");
		return;
	}
	states[current_state].on_exit_actions.push_back(action);
}

/**
 * This function adds an exit action to a specific state in a state machine.
 * 
 * @param action The action parameter is a function pointer to a function that takes no arguments and
 * returns nothing. This function represents the action that will be executed when the state machine
 * exits the specified state.
 * @param state The state parameter is an unsigned 8-bit integer that represents the state to which the
 * exit action is being added.
 */
void StateMachine::add_exit_action(function<void()> action, uint8_t state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine");
		return;
	}
	states[state].on_exit_actions.push_back(action);
}

/**
 * This function adds a transition between two states in a state machine.
 * 
 * @param old_state The current state of the state machine before the transition occurs. It is
 * represented as an unsigned 8-bit integer (uint8_t).
 * @param new_state The new state to transition to. It is of type uint8_t.
 * @param transition The parameter "transition" is a function pointer to a boolean function that takes
 * no arguments. This function represents the condition that must be met in order for the state machine
 * to transition from the old_state to the new_state. If the condition is true, the transition will
 * occur. If the condition is false
 */
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

/**
 * The function adds a nested state machine to the current state machine and clears any timed actions
 * associated with the nested state machine's current state if it is not the current state.
 * 
 * @param state_machine A reference to a StateMachine object that is being added as a nested state
 * machine to the current StateMachine object.
 * @param state The state to which the nested state machine is being added.
 */
void StateMachine::add_state_machine(StateMachine& state_machine, uint8_t state) {
	nested_state_machine[state] = &state_machine;

	if (current_state != state) {
		state_machine.states[current_state].unregister_all_timed_actions();
	}
}

/**
 * The function checks for state transitions and changes the current state if necessary, and also
 * checks for transitions in a nested state machine if applicable.
 */
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

	states[current_state].unregister_all_timed_actions();

	if (nested_state_machine.contains(current_state)) {
		StateMachine* nested_sm = nested_state_machine[current_state];
		nested_sm->states[nested_sm->current_state].unregister_all_timed_actions();
	}

	states[current_state].exit();

	current_state = new_state;

	states[current_state].enter();

	if (nested_state_machine.contains(current_state)) {
		StateMachine* nested_sm = nested_state_machine[current_state];
		nested_sm->current_state = nested_sm->initial_state;
	}

	states[current_state].register_all_timed_actions();
}
