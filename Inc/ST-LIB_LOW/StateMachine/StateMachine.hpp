/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>


class State {
public:
	vector<function<void()>> actions;
	void update();
};

class StateMachine {
public:
	uint8_t initial_state;
	uint8_t current_state;

	map<uint8_t, State> states;
	map<uint8_t, map<uint8_t, function<bool()>>> transitions;
	map<uint8_t, map<uint8_t, vector<function<void()>>>> on_enter;

	void add_update_action(function<void()> action);
	void add_update_action(function<void()> action, uint8_t state);
	void add_enter_action(uint8_t old_state, uint8_t new_state, function<void()> action);
	void add_transition(uint8_t old_state, uint8_t new_state, function<bool()> transition);

	StateMachine(uint8_t initial_state);
	void update();
	void check_transitions();
	void change_state(uint8_t new_state);
};
