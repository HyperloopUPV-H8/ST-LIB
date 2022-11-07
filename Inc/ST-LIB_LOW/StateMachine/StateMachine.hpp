/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>

typedef function<void()> Action;
typedef function<bool()> Transition;

class State{};
class StateMachine {
	uint8_t current_state;

	map<State*, vector<Action>> on_update;
	map<State*, map<State*, Transition>> transitions;
	map<State*, map<State*, Action>> on_enter;

	void add_update_action(Action* action);
	void add_enter_action(State* old_state, State* new_state, Transition transition);
	void add_transition(State* old_state, State* new_state, Transition transition);

};
