/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>


class State {
public:
	vector<function<void()>> on_update_actions = {};
	vector<function<void()>> on_enter_actions = {};
	vector<function<void()>> on_exit_actions = {};
	void update();
	void enter();
	void exit();
};

class StateMachine {
public:
	uint8_t initial_state;
	uint8_t current_state;


	StateMachine() = default;
	StateMachine(uint8_t initial_state);

	void add_state(uint8_t state);
	void add_transition(uint8_t old_state, uint8_t new_state, function<bool()> transition);

	void add_update_action(function<void()> action);
	void add_update_action(function<void()> action, uint8_t state);

	void add_enter_action(function<void()> action);
	void add_enter_action(function<void()> action, uint8_t state);

	void add_exit_action(function<void()> action);
	void add_exit_action(function<void()> action, uint8_t state);

	void update();
	void check_transitions();
	void change_state(uint8_t new_state);

private:
	map<uint8_t, State> states;
	map<uint8_t, map<uint8_t, function<bool()>>> transitions;
};
