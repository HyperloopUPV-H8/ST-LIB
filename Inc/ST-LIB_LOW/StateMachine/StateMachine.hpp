/*
 * Created by Alejandro
 */

#pragma once
#include <C++Utilities/CppUtils.hpp>
#include "Time/Time.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

typedef std::chrono::milliseconds ms;
typedef std::chrono::microseconds us;
typedef std::chrono::seconds s;

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
	typedef uint8_t state_id;

	state_id initial_state = 0;
	state_id current_state = 0;


	StateMachine() = default;
	StateMachine(state_id initial_state);

	void add_state(state_id state);
	void add_transition(state_id old_state, state_id new_state, function<bool()> transition);

	template<class TimeUnit>
	void add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period);
	template<class TimeUnit>
	void add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state);

	void add_enter_action(function<void()> action);
	void add_enter_action(function<void()> action, state_id state);

	void add_exit_action(function<void()> action);
	void add_exit_action(function<void()> action, state_id state);

	void force_change_state(state_id new_state);
	void check_transitions();

	void add_state_machine(StateMachine& state_machine, state_id state);


private:
	unordered_map<state_id, State> states;
	unordered_map<state_id, unordered_map<state_id, function<bool()>>> transitions;

	unordered_map<state_id, StateMachine*> nested_state_machine;
	vector<uint8_t> current_state_timed_actions_in_milliseconds;
	vector<uint8_t> current_state_timed_actions_in_microseconds;
};

template<class TimeUnit>
void StateMachine::add_cyclic_action(function<void()> action, chrono::duration<int64_t, TimeUnit> period, state_id state) {
	if (not states.contains(state)) {
		ErrorHandler("The state %d is not added to the state machine", state);
		return;
	}

	uint32_t microseconds = (uint32_t)chrono::duration_cast<chrono::microseconds>(period).count();
	TimedAction timed_action(action, microseconds);
	states[state].cyclic_actions.push_back(timed_action);

	if (state == current_state) {
		if (microseconds % 1000) {
			optional<uint8_t> optional_id = Time::register_high_precision_alarm(microseconds, action);
			if (not optional_id) {
				ErrorHandler("The high precision alarm could not be registered");
				return;
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
