/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "Time/Time.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

class TimedAction {
public:
	function<void()> action;
	uint32_t microseconds;

	TimedAction() = default;
	TimedAction(function<void()> action, uint32_t microseconds);
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

	template<class TimeUnit>
	void add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, uint8_t state);

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

template<class TimeUnit>
void StateMachine::add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, uint8_t state) {
	if (not state) {
		return; //TODO: Error handler
	}

	uint32_t microseconds = (uint32_t)chrono::duration_cast<chrono::microseconds>(period).count();
	TimedAction timed_action(action, microseconds);
	states[state].cyclic_actions.push_back(timed_action);

	if (state == current_state) {
		if (microseconds % 1000) {
			optional<uint8_t> optional_id = Time::register_high_precision_alarm(microseconds, action);
			if (not optional_id) {
				return; //TODO: Error handler
			}
			current_state_timed_actions_in_microseconds.push_back(optional_id.value());
		}
		else {
			uint8_t id = Time::register_low_precision_alarm(microseconds, action);
			current_state_timed_actions_in_milliseconds.push_back(id);
		}
	}
}

template<class TimeUnit>
void StateMachine::add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period){
	add_cyclic_action(action, period, current_state);
}

#endif
