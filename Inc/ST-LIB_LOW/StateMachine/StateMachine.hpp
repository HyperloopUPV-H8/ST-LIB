/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>

typedef function<void()> Action;
typedef function<bool()> Transition;

class State{
	vector<Action*> actions;
};

class StateMachine {
	State current_state;

	map<State, map<State, Transition*>> transitions;
	map<State, map<State, vector<Action*>>> on_enter;

	void add_update_action(Action* action);
	void add_update_action(Action* action, State state);
	void add_enter_action(State old_state, State new_state, Action* action);
	void add_transition(State old_state, State new_state, Transition* transition);

	void update();
	void check_transitions();
	void change_state(State new_state);

};
