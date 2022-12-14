/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "Time/Time.hpp"

class TimedAction {
public:
	function<void()> action;
	uint32_t microseconds;

	TimedAction() = default;
	TimedAction(function<void()> action, uint32_t microseconds, bool high_precision);
};

class State {
public:
	vector<TimedAction> cyclic_actions = {};
	vector<function<void()>> on_enter_actions = {};
	vector<function<void()>> on_exit_actions = {};
	void enter();
	void exit();
};

class StateMachine {
public:
	uint8_t initial_state = 0;
	uint8_t current_state = 0;


	StateMachine() = default;
	StateMachine(uint8_t initial_state);

	void add_state(uint8_t state);
	void add_transition(uint8_t old_state, uint8_t new_state, function<bool()> transition);

	void add_cyclic_action_in_microseconds(function<void()> action, uint32_t microseconds);
	void add_cyclic_action_in_microseconds(function<void()> action, uint32_t microseconds, uint8_t state);

	void add_cyclic_action_in_milliseconds(function<void()> action, uint32_t milliseconds);
	void add_cyclic_action_in_milliseconds(function<void()> action, uint32_t milliseconds, uint8_t state);

	void add_enter_action(function<void()> action);
	void add_enter_action(function<void()> action, uint8_t state);

	void add_exit_action(function<void()> action);
	void add_exit_action(function<void()> action, uint8_t state);

	void force_change_state(uint8_t new_state);
	void check_transitions();

private:
	unordered_map<uint8_t, State> states;
	unordered_map<uint8_t, unordered_map<uint8_t, function<bool()>>> transitions;

	vector<uint8_t> current_state_timed_actions_in_milliseconds;
	vector<uint8_t> current_state_timed_actions_in_microseconds;
};
